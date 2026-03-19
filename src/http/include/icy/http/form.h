///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup http
/// @{

#pragma once

#include "icy/collection.h"
#include "icy/http/http.h"
#include "icy/packetstream.h"
#include "icy/thread.h"

#include <deque>
#include <memory>


namespace icy {
namespace http {


class HTTP_API Request;
class HTTP_API ConnectionStream;
class HTTP_API FormPart;


//
// HTML Form Writer
//

/// @ingroup http
/// FormWriter is a HTTP client connection adapter for writing HTML forms.
///
/// This class runs in its own thread so as not to block the event loop
/// while uploading big files. Class members are not synchronized hence
/// they should not be accessed while the form is sending, not that there
/// would be any reason to do so.
class HTTP_API FormWriter : public NVCollection
    , public PacketSource
    , public basic::Startable
{
public:
    /// Creates the FormWriter that uses the given connection and
    /// encoding type.
    ///
    /// Encoding must be either "application/x-www-form-urlencoded"
    /// (which is the default) or "multipart/form-data".
    static FormWriter* create(ConnectionStream& conn, const std::string& encoding = FormWriter::ENCODING_URL);

    /// Destroys the FormWriter.
    virtual ~FormWriter();

    /// Adds an part/attachment (file upload) to the form.
    ///
    /// The form takes ownership of the FilePart and deletes it when it
    /// is no longer needed. The part will only be sent if the encoding
    /// set for the form is "multipart/form-data"
    void addPart(const std::string& name, FormPart* part);

    /// Starts the sending thread.
    void start();

    /// Stops the sending thread.
    void stop();

    /// Returns true if the request is complete.
    [[nodiscard]] bool complete() const;

    /// Returns true if the request is cancelled.
    [[nodiscard]] bool cancelled() const;

    /// Prepares the outgoing HTTP request object for submitting the form.
    void prepareSubmit();

    /// Processes the entire stream and calculates the content length.
    /// Not used for chunked encoding.
    uint64_t calculateMultipartContentLength();

    /// Writes "application/x-www-form-urlencoded" encoded data to
    /// the client connection.
    void writeUrl(std::ostream& ostr);

    /// Writes the next multipart "multipart/form-data" encoded
    /// to the client connection. This method is non-blocking,
    /// and is suitable for use with the event loop.
    void writeMultipartChunk();

    /// Called asynchronously by the Runner to write the next message chunk.
    void writeAsync();

    /// Sets the encoding used for posting the form.
    void setEncoding(const std::string& encoding);

    /// Returns the encoding used for posting the form.
    [[nodiscard]] const std::string& encoding() const;

    /// Sets the boundary to use for separating form parts.
    /// Must be set before prepareSubmit() is called.
    void setBoundary(const std::string& boundary);

    /// Returns the MIME boundary used for writing multipart form data.
    [[nodiscard]] const std::string& boundary() const;

    /// The associated HTTP client connection.
    [[nodiscard]] ConnectionStream& connection();

    /// The outgoing packet emitter.
    PacketSignal emitter;

    static const char* ENCODING_URL;               ///< "application/x-www-form-urlencoded"
    static const char* ENCODING_MULTIPART_FORM;    ///< "multipart/form-data"
    static const char* ENCODING_MULTIPART_RELATED; ///< "multipart/related" http://tools.ietf.org/html/rfc2387

protected:
    /// Creates the FormWriter that uses the given encoding.
    FormWriter(ConnectionStream& conn, std::shared_ptr<Runner> runner,
               const std::string& encoding = FormWriter::ENCODING_URL);

    FormWriter(const FormWriter&) = delete;
    FormWriter& operator=(const FormWriter&) = delete;
    FormWriter(FormWriter&&) = delete;
    FormWriter& operator=(FormWriter&&) = delete;

    /// Writes the message boundary std::string, followed
    /// by the message header to the output stream.
    void writePartHeader(const NVCollection& header, std::ostream& ostr);

    /// Writes the final boundary std::string to the output stream.
    void writeEnd(std::ostream& ostr);

    /// Creates a random boundary std::string.
    static std::string createBoundary();

    /// Updates the upload progress via the associated
    /// ConnectionStream object.
    virtual void updateProgress(int nread);

    friend class FormPart;
    friend class FilePart;
    friend class StringPart;

    struct Part
    {
        std::string name;
        std::unique_ptr<FormPart> part;
    };

    using PartQueue = std::deque<Part>;

    ConnectionStream& _stream;
    std::shared_ptr<Runner> _runner;
    std::string _encoding;
    std::string _boundary;
    PartQueue _parts;
    uint64_t _filesLength;
    int _writeState;
    bool _initial;
    bool _complete;
};


//
// Form Part
//

/// @ingroup http
/// An implementation of FormPart.
class HTTP_API FormPart
{
public:
    /// Creates the FormPart with the given MIME type.
    FormPart(const std::string& contentType = "application/octet-stream");

    /// Destroys the FormPart.
    virtual ~FormPart();

    /// Reset the internal state and write position to the start.
    virtual void reset();

    /// Writes a form data chunk to the given HTTP client connection.
    /// Returns true if there is more data to be written.
    virtual bool writeChunk(FormWriter& writer) = 0;

    /// Writes the form data to the given HTTP client connection.
    virtual void write(FormWriter& writer) = 0;

    /// Writes the form data to the given output stream.
    virtual void write(std::ostream& ostr) = 0;

    /// Returns a NVCollection containing additional header
    /// fields for the part.
    [[nodiscard]] NVCollection& headers();

    /// Returns true if this is the initial write.
    [[nodiscard]] virtual bool initialWrite() const;

    /// Returns the MIME type for this part or attachment.
    [[nodiscard]] const std::string& contentType() const;

    /// Returns the length of the current part.
    [[nodiscard]] virtual uint64_t length() const = 0;

protected:
    std::string _contentType;
    uint64_t _length;
    NVCollection _headers;
    bool _initialWrite;
};


//
// File Part
//

/// @ingroup http
/// An implementation of FilePart for plain files.
class HTTP_API FilePart : public FormPart
{
public:
    /// Creates the FilePart for the given path.
    ///
    /// The MIME type is set to application/octet-stream.
    ///
    /// Throws an FileException if the file cannot be opened.
    FilePart(const std::string& path);

    /// Creates the FilePart for the given
    /// path and MIME type.
    ///
    /// Throws an FileException if the file cannot be opened.
    FilePart(const std::string& path, const std::string& contentType);

    /// Creates the FilePart for the given
    /// path and MIME type. The given filename is
    /// used as part filename (see filename()) only.
    ///
    /// Throws an FileException if the file cannot be opened.
    FilePart(const std::string& path, const std::string& filename,
             const std::string& contentType);

    /// Destroys the FilePart.
    virtual ~FilePart();

    /// Opens the file.
    ///
    /// Throws an FileException if the file cannot be opened.
    virtual void open();

    /// Reset the internal state and write position to the start.
    virtual void reset();

    /// Writes a form data chunk to the given HTTP client connection.
    /// Returns true if there is more data to be written.
    bool writeChunk(FormWriter& writer) override;

    /// Writes the form data to the given HTTP client connection.
    void write(FormWriter& writer) override;

    /// Writes the form data to the given output stream.
    void write(std::ostream& ostr) override;

    /// Returns the filename portion of the path.
    [[nodiscard]] const std::string& filename() const;

    /// Returns the file input stream.
    [[nodiscard]] std::ifstream& stream();

    /// Returns the length of the current part.
    [[nodiscard]] uint64_t length() const override;

protected:
    std::string _path;
    std::string _filename;
    std::ifstream _istr;
    uint64_t _fileSize;
};


//
// String Part
//

/// @ingroup http
/// An implementation of StringPart for plain files.
class HTTP_API StringPart : public FormPart
{
public:
    /// Creates the StringPart for the given string.
    StringPart(const std::string& path);

    /// Creates the StringPart for the given string and MIME type.
    StringPart(const std::string& data, const std::string& contentType);

    /// Destroys the StringPart.
    virtual ~StringPart();

    /// Writes a form data chunk to the given HTTP client connection.
    /// Returns true if there is more data to be written.
    bool writeChunk(FormWriter& writer) override;

    /// Writes the form data to the given HTTP client connection.
    void write(FormWriter& writer) override;

    /// Writes the form data to the given output stream.
    void write(std::ostream& ostr) override;

    /// Returns the length of the current part.
    [[nodiscard]] uint64_t length() const override;

protected:
    std::string _data;
};


} // namespace http
} // namespace icy


/// @}
