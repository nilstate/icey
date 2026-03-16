///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{


#include "scy/http/form.h"
#include "scy/crypto/crypto.h"
#include "scy/http/client.h"

#include "scy/filesystem.h"
#include "scy/http/packetizers.h"
#include "scy/http/url.h"
#include "scy/idler.h"
#include <memory>
#include <stdexcept>


namespace scy {
namespace http {


//
// Form Writer
//


const char* FormWriter::ENCODING_URL = "application/x-www-form-urlencoded";
const char* FormWriter::ENCODING_MULTIPART_FORM = "multipart/form-data";
const char* FormWriter::ENCODING_MULTIPART_RELATED = "multipart/related";

const int FILE_CHUNK_SIZE = 65536;


FormWriter* FormWriter::create(ConnectionStream& stream, const std::string& encoding)
{
    std::unique_ptr<http::FormWriter> wr(new http::FormWriter(stream, std::make_shared<Idler>(stream.connection()->socket()->loop()), encoding));
    auto* rawWr = wr.get();
    stream.Outgoing.attachSource(wr.release(), true, true);
    if (stream.connection()->request().isChunkedTransferEncoding()) {
        if (encoding == http::FormWriter::ENCODING_URL)
            throw std::invalid_argument("FormWriter::create: URL encoding cannot use chunked transfer");
        if (stream.connection()->request().getVersion() == http::Message::HTTP_1_0)
            throw std::invalid_argument("FormWriter::create: HTTP/1.0 does not support chunked transfer");
        auto chunked = std::make_unique<http::ChunkedAdapter>(stream.connection());
        stream.Outgoing.attach(chunked.release(), -1, true);
    }
    stream.Outgoing.lock();
    return rawWr;
}


FormWriter::FormWriter(ConnectionStream& connection, std::shared_ptr<Runner> runner, const std::string& encoding)
    : PacketSource(this->emitter)
    , _stream(connection)
    , _runner(runner)
    , _encoding(encoding)
    , _filesLength(0)
    , _writeState(0)
    , _initial(true)
    , _complete(false)
{
}


FormWriter::~FormWriter()
{
    // Parts are now managed by unique_ptr, no manual delete needed
}


void FormWriter::addPart(const std::string& name, FormPart* part)
{
    if (!part) {
        throw std::runtime_error("FormWriter::addPart: null part");
    }
    if (_encoding == ENCODING_URL) {
        throw std::runtime_error("FormWriter::addPart: cannot add parts with URL encoding");
    }

    Part p;
    p.part.reset(part);
    p.name = name;
    _filesLength += part->length();
    _parts.push_back(std::move(p));
}


void FormWriter::prepareSubmit()
{
    http::Request& request = _stream.connection()->request();
    if (request.getMethod() == http::Method::Post ||
        request.getMethod() == http::Method::Put) {
        if (_encoding == ENCODING_URL) {
            request.setContentType(_encoding);
            request.setChunkedTransferEncoding(false);
            std::ostringstream ostr;
            writeUrl(ostr);
            if (ostr.tellp() <= 0)
                throw std::runtime_error("FormWriter::prepareSubmit: empty URL-encoded form body");
            request.setContentLength(ostr.tellp());
        } else {
            if (_boundary.empty())
                _boundary = createBoundary();
            std::string ct(_encoding);
            ct.append("; boundary=\"");
            ct.append(_boundary);
            ct.append("\"");
            request.setContentType(ct);

            // Set the total file size for upload progress updates.
            if (!_parts.empty()) {
                if (!_filesLength)
                    LWarn("FormWriter::prepareSubmit: parts present but total file length is zero");
                _stream.OutgoingProgress.total = _filesLength;
            }

            // Set Content-Length for non-chunked transfers
            if (!request.isChunkedTransferEncoding() &&
                request.getVersion() != http::Message::HTTP_1_0)
                request.setContentLength(calculateMultipartContentLength());
        }
        if (request.getVersion() == http::Message::HTTP_1_0) {
            request.setKeepAlive(false);
            request.setChunkedTransferEncoding(false);
        }
    } else {
        std::string uri = request.getURI();
        std::ostringstream ostr;
        writeUrl(ostr);
        uri.append("?");
        uri.append(ostr.str());
        request.setURI(uri);
    }
}


void FormWriter::start()
{
    prepareSubmit();

    _runner->setRepeating(true);
    _runner->start(std::bind(&FormWriter::writeAsync, this));
}


void FormWriter::stop()
{
    _runner->cancel();
}


uint64_t FormWriter::calculateMultipartContentLength()
{
    std::ostringstream ostr;
    for (const auto& [key, value] : *this) {
        NVCollection header;
        if (_encoding == ENCODING_MULTIPART_FORM) {
            std::string disp("form-data; name=\"");
            disp.append(key);
            disp.append("\"");
            header.set("Content-Disposition", disp);
        }
        writePartHeader(header, ostr);
        ostr << value;
    }
    for (auto& p : _parts) {
        NVCollection header(p.part->headers());
        if (_encoding == ENCODING_MULTIPART_FORM) {
            std::string disp("form-data; name=\"");
            disp.append(p.name);
            disp.append("\"");
            auto filePart = dynamic_cast<http::FilePart*>(p.part.get());
            if (filePart) {
                std::string filename = filePart->filename();
                if (!filename.empty()) {
                    disp.append("; filename=\"");
                    disp.append(filename);
                    disp.append("\"");
                }
            }
            header.set("Content-Disposition", disp);
        }
        header.set("Content-Type", p.part->contentType());
        writePartHeader(header, ostr);
        p.part->write(ostr);
        p.part->reset(); // reset part state
    }
    writeEnd(ostr);
    return ostr.tellp();
}


void FormWriter::writeAsync()
{
    try {
        if (complete())
            throw std::runtime_error("FormWriter::writeAsync: already complete");
        if (encoding() == ENCODING_URL) {
            std::ostringstream ostr;
            writeUrl(ostr);
            emit(ostr.str());
            _complete = true;
        } else
            writeMultipartChunk();
        if (complete())
            _runner->cancel();
    } catch (std::exception&) {
        _runner->cancel();
        throw;
    }
}


void FormWriter::writeMultipartChunk()
{
    switch (_writeState) {

        // Send form values
        case 0:
            for (const auto& [key, value] : *this) {
                std::ostringstream ostr;
                NVCollection header;
                if (_encoding == ENCODING_MULTIPART_FORM) {
                    std::string disp("form-data; name=\"");
                    disp.append(key);
                    disp.append("\"");
                    header.set("Content-Disposition", disp);
                }
                writePartHeader(header, ostr);
                ostr << value;
                emit(ostr.str());
            }
            _writeState++;
            break;

        // Send file parts
        case 1:
            if (!_parts.empty()) {
                auto& p = _parts.front();

                if (p.part->initialWrite()) {
                    std::ostringstream ostr;
                    NVCollection header(p.part->headers());
                    if (_encoding == ENCODING_MULTIPART_FORM) {
                        std::string disp("form-data; name=\"");
                        disp.append(p.name);
                        disp.append("\"");
                        auto filePart = dynamic_cast<http::FilePart*>(p.part.get());
                        if (filePart) {
                            std::string filename = filePart->filename();
                            if (!filename.empty()) {
                                disp.append("; filename=\"");
                                disp.append(filename);
                                disp.append("\"");
                            }
                        }
                        header.set("Content-Disposition", disp);
                    }
                    header.set("Content-Type", p.part->contentType());
                    writePartHeader(header, ostr);
                    emit(ostr.str());
                }
                if (p.part->writeChunk(*this)) {
                    return; // return after writing a chunk
                } else {
                    _parts.pop_front();
                }
            }
            if (_parts.empty())
                _writeState++;
            break;

        // Send final packet
        case 2: {
            std::ostringstream ostr;
            writeEnd(ostr);
            emit(ostr.str());

            // Write chunked transfer-encoding terminator
            if (_stream.connection()->request().isChunkedTransferEncoding()) {
                emit("0\r\n\r\n", 5,
                     PacketFlags::NoModify | PacketFlags::Final);
            }

            _complete = true;
            _writeState = -1; // raise error if called again
        } break;

        // Invalid state
        default:
            LError("Invalid write state: ", _writeState);
            throw std::runtime_error("FormWriter: invalid write state");
            break;
    }
}


void FormWriter::writeUrl(std::ostream& ostr)
{
    bool first = true;
    for (const auto& [key, value] : *this) {
        if (!first)
            ostr << "&";
        first = false;
        ostr << URL::encode(key) << "=" << URL::encode(value);
    }
}


void FormWriter::writePartHeader(const NVCollection& header, std::ostream& ostr)
{
    if (_initial)
        _initial = false;
    else
        ostr << "\r\n";
    ostr << "--" << _boundary << "\r\n";

    for (const auto& [name, value] : header) {
        ostr << name << ": " << value << "\r\n";
    }
    ostr << "\r\n";
}


void FormWriter::writeEnd(std::ostream& ostr)
{
    ostr << "\r\n--" << _boundary << "--\r\n";
}


void FormWriter::updateProgress(int nread)
{
    _stream.OutgoingProgress.update(nread);
}


std::string FormWriter::createBoundary()
{
    return "boundary_" + util::randomString(8);
}


void FormWriter::setEncoding(const std::string& encoding)
{
    _encoding = encoding;
}


void FormWriter::setBoundary(const std::string& boundary)
{
    _boundary = boundary;
}


const std::string& FormWriter::encoding() const
{
    return _encoding;
}


const std::string& FormWriter::boundary() const
{
    return _boundary;
}


ConnectionStream& FormWriter::connection()
{
    return _stream;
}


bool FormWriter::complete() const
{
    return _complete;
}


bool FormWriter::cancelled() const
{
    return _runner->cancelled();
}


//
// File Part Source
//


FormPart::FormPart(const std::string& contentType)
    : _contentType(contentType)
    , _initialWrite(true)
{
}


FormPart::~FormPart()
{
}


NVCollection& FormPart::headers()
{
    return _headers;
}


const std::string& FormPart::contentType() const
{
    return _contentType;
}


bool FormPart::initialWrite() const
{
    return _initialWrite;
}


void FormPart::reset()
{
    _initialWrite = true;
}


//
// File Part Source
//


FilePart::FilePart(const std::string& path)
    : _path(path)
    , _filename(fs::filename(path))
    , _fileSize(0)
{
    open();
}


FilePart::FilePart(const std::string& path, const std::string& contentType)
    : FormPart(contentType)
    , _path(path)
    , _filename(fs::filename(path))
    , _fileSize(0)
{
    open();
}


FilePart::FilePart(const std::string& path, const std::string& filename,
                   const std::string& contentType)
    : FormPart(contentType)
    , _path(path)
    , _filename(filename)
    , _fileSize(0)
{
    open();
}


FilePart::~FilePart()
{
}


void FilePart::open()
{
    _istr.open(_path.c_str(), std::ios::in | std::ios::binary);
    if (!_istr.is_open())
        throw std::runtime_error("Cannot open file: " + _path);

    // Get file size
    _istr.seekg(0, std::ios::end);
    _fileSize = _istr.tellg();
    _istr.seekg(0, std::ios::beg);
}


void FilePart::reset()
{
    FormPart::reset();
    _istr.clear();
    _istr.seekg(0, std::ios::beg);
}


bool FilePart::writeChunk(FormWriter& writer)
{
    if (writer.cancelled())
        return false;
    _initialWrite = false;

    char buffer[FILE_CHUNK_SIZE];
    if (_istr.read(buffer, FILE_CHUNK_SIZE)) {
        writer.emit(buffer, static_cast<size_t>(_istr.gcount()));
        writer.updateProgress(static_cast<int>(_istr.gcount()));
        return true;
    }

    if (_istr.eof()) {
        // Still a few bytes left to write?
        if (_istr.gcount() > 0) {
            writer.emit(buffer, static_cast<size_t>(_istr.gcount()));
            writer.updateProgress(static_cast<int>(_istr.gcount()));
        }
        return false; // all done
    }

    if (_istr.bad()) {
        throw std::runtime_error("Cannot read multipart source file: " + _filename);
    }
    return false;
}


void FilePart::write(FormWriter& writer)
{
    _initialWrite = false;

    char buffer[FILE_CHUNK_SIZE];
    while (_istr.read(buffer, FILE_CHUNK_SIZE) && !writer.cancelled()) {
        writer.emit(buffer, static_cast<size_t>(_istr.gcount()));
        writer.updateProgress(static_cast<int>(_istr.gcount()));
    }

    if (_istr.eof()) {
        if (_istr.gcount() > 0 && !writer.cancelled()) {
            writer.emit(buffer, static_cast<size_t>(_istr.gcount()));
            writer.updateProgress(static_cast<int>(_istr.gcount()));
        }
    } else if (_istr.bad())
        throw std::runtime_error("Cannot read multipart source file: " +
                                 _filename);
}


void FilePart::write(std::ostream& ostr)
{
    _initialWrite = false;

    char buffer[FILE_CHUNK_SIZE];
    while (_istr.read(buffer, FILE_CHUNK_SIZE))
        ostr.write(buffer, static_cast<size_t>(_istr.gcount()));

    if (_istr.eof()) {
        if (_istr.gcount() > 0)
            ostr.write(buffer, static_cast<size_t>(_istr.gcount()));
    } else if (_istr.bad())
        throw std::runtime_error("Cannot read multipart source file: " +
                                 _filename);
}


const std::string& FilePart::filename() const
{
    return _filename;
}


std::ifstream& FilePart::stream()
{
    return _istr;
}


uint64_t FilePart::length() const
{
    return _fileSize;
}


//
// String Part Source
//


StringPart::StringPart(const std::string& data)
    : FormPart("application/octet-stream")
    , _data(data)
{
}


StringPart::StringPart(const std::string& data, const std::string& contentType)
    : FormPart(contentType)
    , _data(data)
{
}


StringPart::~StringPart()
{
}


bool StringPart::writeChunk(FormWriter& writer)
{
    _initialWrite = false;

    writer.emit(_data.c_str(), _data.length());
    writer.updateProgress(static_cast<int>(_data.length()));

    return false;
}


void StringPart::write(FormWriter& writer)
{
    _initialWrite = false;

    writer.emit(_data.c_str(), _data.length());
    writer.updateProgress(static_cast<int>(_data.length()));
}


void StringPart::write(std::ostream& ostr)
{
    _initialWrite = false;

    ostr.write(_data.c_str(), _data.length());
}


uint64_t StringPart::length() const
{
    return _data.length();
}


} // namespace http
} // namespace scy


/// @}


// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
