#include "basetests.h"

using std::cerr;
using std::cout;
using namespace icy;
using namespace icy::test;


int main(int argc, char** argv)
{
    // Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();

    // =========================================================================
    // Signal Benchmarks
    //
    describe("signal class member benchmark", []() {
        Signal<void(uint64_t&)> signal;
        SignalCounter counter;
        signal += slot(&counter, &SignalCounter::increment);
        const uint64_t benchstart = time::hrtime();
        uint64_t i, value = 0;
        for (i = 0; i < 999999; i++) {
            signal.emit(value);
        }
        const uint64_t benchdone = time::hrtime();
        expect(value == i);

        std::cout << "signal class member benchmark: "
                  << ((benchdone - benchstart) * 1.0 / i) << "ns "
                  << "per emission (sz=" << sizeof(signal) << ")"
                  << '\n';
    });

    describe("signal const class member benchmark", []() {
        Signal<void(uint64_t&)> signal;
        SignalCounter counter;
        signal += slot(&counter, &SignalCounter::incrementConst);
        const uint64_t benchstart = time::hrtime();
        uint64_t i, value = 0;
        for (i = 0; i < 999999; i++) {
            signal.emit(value);
        }
        const uint64_t benchdone = time::hrtime();
        expect(value == i);

        std::cout << "signal const class member benchmark: "
                  << ((benchdone - benchstart) * 1.0 / i) << "ns "
                  << "per emission (sz=" << sizeof(signal) << ")" << '\n';
    });

    describe("signal static member benchmark", []() {
        Signal<void(uint64_t&)> signal;
        signal += slot(&SignalCounter::incrementStatic);
        const uint64_t benchstart = time::hrtime();
        uint64_t i, value = 0;
        for (i = 0; i < 999999; i++) {
            signal.emit(value);
        }
        const uint64_t benchdone = time::hrtime();
        expect(value == i);

        std::cout << "signal static member benchmark: "
                  << ((benchdone - benchstart) * 1.0 / i) << "ns "
                  << "per emission (sz=" << sizeof(signal) << ")"
                  << '\n';
    });

    describe("signal free function benchmark", []() {
        Signal<void(uint64_t&)> signal;
        signal += signalIncrementFree;
        const uint64_t benchstart = time::hrtime();
        uint64_t i, value = 0;
        for (i = 0; i < 999999; i++) {
            signal.emit(value);
        }
        const uint64_t benchdone = time::hrtime();
        expect(value == i);

        std::cout << "signal free function benchmark: "
                  << ((benchdone - benchstart) * 1.0 / i) << "ns "
                  << "per emission (sz=" << sizeof(signal) << ")"
                  << '\n';
    });

    describe("retained request keeps state alive until completion", []() {
        bool invoked = false;
        auto state = std::make_shared<int>(42);
        std::weak_ptr<int> weak = state;

        auto& req = uv::createRetainedRequest<uv::ConnectReq>(
            state,
            [&](const std::shared_ptr<int>& retained, const uv::BasicEvent& event) {
                expect(event.status == 0);
                expect(*retained == 42);
                invoked = true;
                state.reset();
                expect(!weak.expired());
            });

        state.reset();
        expect(!weak.expired());

        uv::ConnectReq::defaultCallback(&req.req, 0);

        expect(invoked);
        expect(weak.expired());
    });


    // =========================================================================
    // Buffer
    //
    describe("buffer", []() {
        ByteOrder orders[2] = {ByteOrder::Host, ByteOrder::Network};
        for (size_t i = 0; i < 2; i++) {
            Buffer buffer(1024);
            BitReader reader(buffer, orders[i]);
            BitWriter writer(buffer, orders[i]);
            expect(orders[i] == reader.order());
            expect(orders[i] == writer.order());

            // Write and read uint8_t.
            uint8_t wu8 = 1;
            writer.putU8(wu8);
            uint8_t ru8;
            reader.getU8(ru8);
            expect(writer.position() == 1);
            expect(reader.position() == 1);
            expect(wu8 == ru8);

            // Write and read uint16_t.
            uint16_t wu16 = (1 << 8) + 1;
            writer.putU16(wu16);
            uint16_t ru16;
            reader.getU16(ru16);
            expect(wu16 == ru16);
            expect(writer.position() == 3);
            expect(reader.position() == 3);

            // Write and read UInt24.
            uint32_t wu24 = (3 << 16) + (2 << 8) + 1;
            writer.putU24(wu24);
            uint32_t ru24;
            reader.getU24(ru24);
            expect(wu24 == ru24);
            expect(writer.position() == 6);
            expect(reader.position() == 6);

            // Write and read uint32_t.
            uint32_t wu32 = (4 << 24) + (3 << 16) + (2 << 8) + 1;
            writer.putU32(wu32);
            uint32_t ru32;
            reader.getU32(ru32);
            expect(wu32 == ru32);
            expect(writer.position() == 10);
            expect(reader.position() == 10);

            // Write and read uint64_t.
            uint32_t another32 = (8 << 24) + (7 << 16) + (6 << 8) + 5;
            uint64_t wu64 = (static_cast<uint64_t>(another32) << 32) + wu32;
            writer.putU64(wu64);
            uint64_t ru64;
            reader.getU64(ru64);
            expect(wu64 == ru64);
            expect(writer.position() == 18);
            expect(reader.position() == 18);

            // Write and read string.
            std::string write_string("hello");
            writer.put(write_string);
            std::string read_string;
            reader.get(read_string, write_string.size());
            expect(write_string == read_string);
            expect(writer.position() == 23);
            expect(reader.position() == 23);

            // Write and read bytes
            char write_bytes[] = "foo";
            writer.put(write_bytes, 3);
            char read_bytes[3];
            reader.get(read_bytes, 3);
            for (int x = 0; x < 3; ++x) {
                expect(write_bytes[x] == read_bytes[x]);
            }
            expect(writer.position() == 26);
            expect(reader.position() == 26);

            // try {
            //     reader.getU8(ru8);
            //     expect(0 && "must throw");
            // }
            // catch (std::out_of_range& exc) {
            // }
        }
    });

    describe("buffer move offset", []() {
        Buffer buffer(1000);

        BitWriter writer(buffer.data(), buffer.size());
        expect(writer.position() == 0);
        expect(writer.limit() == buffer.size());

        // Move the write cursor around.
        writer.seek(100);
        expect(writer.position() == 100);
        writer.seek(900);
        expect(writer.position() == 900);
        writer.seek(500);
        expect(writer.position() == 500);

        // The cursor can be set to past the end of the buffer,
        // but can't perform any write operations there.
        try {
            writer.seek(1200);
            expect(0 && "must throw");
        } catch (std::out_of_range& exc) {
        }
    });

    describe("buffer write chunk", []() {
        const std::string kInputData = "abcdefghijklmnopqrstuvwxyz";
        Buffer output(kInputData.size());
        BitWriter writer(output.data(), output.size());

        writer.put(kInputData.data(), kInputData.size());
        expect(writer.position() == output.size());

        // Compare input and output data, converting the latter to
        // a string for clarity of error messages.
        expect(std::string(output.begin(), output.end()) == kInputData);
    });


    describe("dynamic resizable buffer", []() {
        std::string write_string("hello");
        std::string write_string1("world");

        ByteOrder orders[2] = {ByteOrder::Host, ByteOrder::Network};
        for (size_t i = 0; i < 2; i++) {
            Buffer buffer;
            DynamicBitWriter writer(buffer, orders[i]);
            writer.put(write_string.c_str(), write_string.size());
            writer.put(write_string1.c_str(), write_string1.size());

            BitReader reader(buffer, orders[i]);
            std::string read_string;
            reader.get(read_string, write_string.size() + write_string1.size());

            expect((write_string + write_string1) == read_string);
            expect(writer.position() == 10);
            expect(reader.position() == 10);
        }
    });


    // =========================================================================
    // Collection
    //
    describe("collection", []() {
        NVCollection nvc;
        expect(nvc.empty());
        expect(nvc.size() == 0);

        nvc.set("name", "value");
        expect(!nvc.empty());
        expect(nvc["name"] == "value");
        expect(nvc["Name"] == "value");

        nvc.set("name2", "value2");
        expect(nvc.get("name2") == "value2");
        expect(nvc.get("NAME2") == "value2");

        expect(nvc.size() == 2);

        try {
            std::string value = nvc.get("name3");
            expect(0 && "not found - must throw");
        } catch (std::exception&) {
        }

        try {
            std::string value = nvc["name3"];
            expect(0 && "not found - must throw");
        } catch (std::exception&) {
        }

        expect(nvc.get("name", "default") == "value");
        expect(nvc.get("name3", "default") == "default");

        expect(nvc.has("name"));
        expect(nvc.has("name2"));
        expect(!nvc.has("name3"));

        nvc.add("name3", "value3");
        expect(nvc.get("name3") == "value3");
        nvc.add("name3", "value31");
        nvc.add("Connection", "value31");

        NVCollection::ConstIterator it = nvc.find("Name3");
        expect(it != nvc.end());
        std::string v1 = it->second;
        expect(it->first == "name3");
        ++it;
        expect(it != nvc.end());
        std::string v2 = it->second;
        expect(it->first == "name3");

        expect((v1 == "value3" && v2 == "value31") || (v1 == "value31" && v2 == "value3"));

        nvc.erase("name3");
        expect(!nvc.has("name3"));
        expect(nvc.find("name3") == nvc.end());

        it = nvc.begin();
        expect(it != nvc.end());
        ++it;
        expect(it != nvc.end());
        // ++it;
        // expect(it == nvc.end());

        nvc.clear();
        expect(nvc.empty());

        expect(nvc.size() == 0);
    });


    // =========================================================================
    // Filesystem
    //
    describe("filesystem", []() {
        std::string path(icy::getExePath());
        // cout << "executable path: " << path << endl;
        expect(fs::exists(path));

        std::string junkPath(path + "junkname.huh");
        // cout << "junk path: " << junkPath << endl;
        expect(!fs::exists(junkPath));

        std::string dir(fs::dirname(path));
        // cout << "dir name: " << dir << endl;
        expect(fs::exists(dir));
        expect(fs::exists(dir + "/"));
        expect(fs::dirname("file.a") == ".");
        expect(fs::dirname("/some/file.a") == "/some");
        expect(fs::dirname("/some/dir") == "/some");
        expect(fs::dirname("/some/dir/") == "/some/dir");

        std::string test(fs::dirname(dir));
        fs::addnode(test, "tests");
        expect(test == dir);
    });

    // =========================================================================
    // Logger

    describe("logger", []() {
        Logger& logger = Logger::instance();

        // Test default synchronous writer
        logger.setWriter(std::make_unique<LogWriter>());
        clock_t start = clock();
        for (unsigned i = 0; i < 1000; i++)
            LTrace("sync log ", i);
        cout
            << "logger: synchronous test completed after: " << (clock() - start) << "ms" << '\n';

        // Test default synchronous var args writer
        start = clock();
        for (unsigned i = 0; i < 1000; i++)
            LTrace("sync log", i);
        cout
            << "logger: synchronous var args test completed after: " << (clock() - start) << "ms" << '\n';

        // Test asynchronous writer (approx 10x faster with console output)
        logger.setWriter(std::make_unique<AsyncLogWriter>());
        start = clock();
        for (unsigned i = 0; i < 1000; i++)
            LTrace("async log ", i);
        cout
            << "logger: asynchronous test completed after: " << (clock() - start) << "ms" << '\n';

        //// Test function logging
        //start = clock();
        //for (unsigned i = 0; i < 1000; i++)
        //    LTrace("test: ", i);
        //cout << "logger: asynchronous function logging completed after: "
        //     << (clock() - start) << endl;

        //// Test function and mem address logging
        //start = clock();
        //for (unsigned i = 0; i < 1000; i++)
        //    LTrace("test: ", i);
        //cout << "logger: asynchronous function and mem address logging completed after: "
        //     << (clock() - start) << endl;

        logger.setWriter(nullptr);

        // logger.getDefault()->setFilter("icy::*");
        // Destory the current Logger instance to ensure no crash
        Logger::destroy();
    });


    // =========================================================================
    // Platform
    //
    describe("platform", []() {
        expect(!icy::getExePath().empty());
        expect(!icy::getCwd().empty());

        // cout << "executable path: " << icy::getExePath() << endl;
        // cout << "current working directory: " << icy::getCwd() << endl;
    });


    // =========================================================================
    // Version String Comparison
    //
    describe("version string comparison", []() {
        expect((util::Version("3.7.8.0") == util::Version("3.7.8.0")) == true);
        expect((util::Version("3.7.8.0") == util::Version("3.7.8")) == true);
        expect((util::Version("3.7.8.0") < util::Version("3.7.8")) == false);
        expect((util::Version("3.7.9") < util::Version("3.7.8")) == false);
        // expect((util::Version("3") < util::Version("3.7.9")) == true);
        expect((util::Version("1.7.9") < util::Version("3.1")) == true);

        cout << "Printing version (3.7.8.0): " << util::Version("3.7.8.0") << '\n';
    });


    // =========================================================================
    // Idler
    //
    describe("idler", []() {
        int counter = 0;
        bool ran = false;

        // Create the idler with a lambda
        Idler idler;
        idler.start([&]() {
            // std::cout << "on idle: " << counter << std::endl;
            if (++counter == 10) {
                ran = true;
                expect(idler.running() == true);
                idler.cancel();
                expect(idler.cancelled() == true);
            }
        });

        // Make the idler reference the event loop
        idler.handle().ref();

        // Run the loop
        uv::runLoop();

        // Check variables
        expect(ran == true);
        expect(counter == 10);
        expect(idler.running() == false);

        // Reinitialize the idler with veradic args
        ran = false;
        idler.start([](Idler* idler, bool* ran) {
            *ran = true;
            expect(idler->running() == true);
            idler->cancel();
        },
                    &idler, &ran);

        // Run the loop again
        uv::runLoop();

        // Check variables
        expect(ran == true);
        expect(idler.running() == false);

        // Bind to class member
        // idler.start(std::bind(&IdlerTest::idlerCallback, this));
    });


    // =========================================================================
    // Thread
    //
    describe("thread", []() {
        bool ran = false;
        Thread t1;
        t1.start([&]() {
            ran = true;
            expect(t1.running() == true);
        });
        t1.join();
        expect(ran == true);
        expect(t1.running() == false);

        // Reuse the same thread container
        ran = false;
        t1.start([&]() {
            ran = true;
            expect(t1.running() == true);
        });
        t1.join();
        expect(ran == true);
        expect(t1.running() == false);
    });


    // =========================================================================
    // Process
    //
    describe("process", []() {
        bool gotStdout = false, gotExit = false;
        Process proc{"echo", "hello world"};
        proc.onstdout = [&](std::string line) {
            gotStdout = true;
            proc.kill();
        };
        proc.onexit = [&](int64_t status) {
            gotExit = true;
        };
        proc.spawn();
        uv::runLoop();
        expect(gotStdout);
        expect(gotExit);
    });

    describe("process with env", []() {
        std::string output;
        bool gotExit = false;
        Process proc{"sh", "-c", "echo $TEST_VAR"};
        proc.env = {"TEST_VAR=hello_from_env"};
        proc.onstdout = [&](std::string line) {
            output += line;
        };
        proc.onexit = [&](int64_t status) {
            gotExit = true;
        };
        proc.spawn();
        uv::runLoop();
        expect(gotExit);
        expect(output.find("hello_from_env") != std::string::npos);
    });

    describe("process stderr", []() {
        std::string errOutput;
        bool gotExit = false;
        Process proc{"sh", "-c", "echo error >&2"};
        proc.onstderr = [&](std::string line) {
            errOutput += line;
        };
        proc.onexit = [&](int64_t status) {
            gotExit = true;
        };
        proc.spawn();
        uv::runLoop();
        expect(gotExit);
        expect(errOutput.find("error") != std::string::npos);
    });


    // =========================================================================
    // Buffer overflow
    //
    describe("buffer overflow", []() {
        Buffer buffer(8);
        BitReader reader(buffer, ByteOrder::Host);
        BitWriter writer(buffer, ByteOrder::Host);

        // Fill the buffer
        writer.putU64(0x0102030405060708ULL);

        // Attempting to read past the end should throw
        uint64_t val;
        reader.getU64(val);
        expect(val == 0x0102030405060708ULL);

        try {
            uint8_t extra;
            reader.getU8(extra);
            expect(0 && "reading past end - must throw");
        } catch (std::out_of_range&) {
        }

        // Attempting to write past the end should throw
        try {
            writer.putU8(0xFF);
            expect(0 && "writing past end - must throw");
        } catch (std::out_of_range&) {
        }
    });


    // =========================================================================
    // Random
    //
    describe("random", []() {
        Random rng;

        // next() should produce different values
        auto a = rng.next();
        auto b = rng.next();
        auto c = rng.next();
        // Extremely unlikely all three are the same
        expect(!(a == b && b == c));

        // next(n) should produce values in range [0, n)
        for (int i = 0; i < 100; ++i) {
            expect(rng.next(10) < 10);
        }

        // nextBool() should return true and false (statistical)
        int trueCount = 0;
        for (int i = 0; i < 100; ++i) {
            if (rng.nextBool())
                trueCount++;
        }
        expect(trueCount > 0 && trueCount < 100);

        // nextFloat() should be in [0, 1)
        for (int i = 0; i < 100; ++i) {
            auto f = rng.nextFloat();
            expect(f >= 0.0f && f < 1.0f);
        }

        // nextDouble() should be in [0, 1)
        for (int i = 0; i < 100; ++i) {
            auto d = rng.nextDouble();
            expect(d >= 0.0 && d < 1.0);
        }

        // Seeding with same value should produce same sequence
        Random rng1, rng2;
        rng1.seed(42);
        rng2.seed(42);
        for (int i = 0; i < 10; ++i) {
            expect(rng1.next() == rng2.next());
        }
    });


    // =========================================================================
    // Base64 encode/decode
    //
    describe("base64 encode/decode", []() {
        // RFC 4648 test vectors
        base64::Encoder enc;
        base64::Decoder dec;

        // Empty string
        {
            std::string in;
            std::string encoded;
            enc.encode(in, encoded);
            expect(encoded.empty());
        }

        {
            std::vector<std::uint8_t> emptyBytes;
            expect(base64::encode(emptyBytes).empty());
        }

        // Standard test vectors via stream interface
        {
            std::istringstream in("f");
            std::ostringstream out;
            base64::Encoder enc2;
            enc2.setLineLength(0);
            enc2.encode(in, out);
            std::string result = out.str();
            // Remove trailing newline/padding whitespace
            while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
                result.pop_back();
            expect(result == "Zg==");
        }

        // Round-trip with longer data
        {
            std::string original = "The quick brown fox jumps over the lazy dog";
            base64::Encoder enc3;
            enc3.setLineLength(0);
            std::string encoded;
            enc3.encode(original, encoded);
            expect(!encoded.empty());

            std::string decoded = base64::decode(encoded);
            expect(decoded == original);
        }

        // Round-trip with binary data (all byte values)
        {
            std::string binary;
            for (int i = 0; i < 256; ++i)
                binary += static_cast<char>(i);

            base64::Encoder enc4;
            enc4.setLineLength(0);
            std::string encoded;
            enc4.encode(binary, encoded);

            std::string decoded = base64::decode(encoded);
            expect(decoded == binary);
        }
    });


    // =========================================================================
    // DateTime / Timestamp
    //
    describe("datetime", []() {
        // Timestamp should be non-zero and increasing
        Timestamp ts1;
        Timestamp ts2;
        expect(ts1.epochMicroseconds() > 0);
        expect(ts2 >= ts1);

        // DateTime construction
        DateTime dt(2024, 1, 15, 10, 30, 45);
        expect(dt.year() == 2024);
        expect(dt.month() == 1);
        expect(dt.day() == 15);
        expect(dt.hour() == 10);
        expect(dt.minute() == 30);
        expect(dt.second() == 45);

        // Timespan
        Timespan ts(0, 1, 30, 0, 0); // 1 hour 30 minutes
        expect(ts.totalSeconds() == 5400);
        expect(ts.totalMinutes() == 90);
        expect(ts.hours() == 1);
        expect(ts.minutes() == 30);
    });


    // =========================================================================
    // Error
    //
    describe("error", []() {
        Error err;
        expect(!err.any());
        expect(err.err == 0);
        expect(err.message.empty());

        err.err = 42;
        err.message = "test error";
        expect(err.any());

        err.reset();
        expect(!err.any());
        expect(err.err == 0);
        expect(err.message.empty());
    });


    // =========================================================================
    // Stateful
    //
    describe("stateful", []() {
        State s1(0);
        expect(s1.id() == 0);

        s1.set(5);
        expect(s1.id() == 5);
        expect(s1.equals(5));
        expect(s1.between(3, 7));
        expect(!s1.between(6, 10));

        State s2(5);
        expect(s1 == s2);
    });


    // =========================================================================
    // Util: string operations
    //
    describe("util string operations", []() {
        // split
        auto parts = util::split("a,b,c", ",");
        expect(parts.size() == 3);
        expect(parts[0] == "a");
        expect(parts[1] == "b");
        expect(parts[2] == "c");

        // split with limit
        parts = util::split("a,b,c,d", ",", 2);
        expect(parts.size() == 2);

        // trim
        expect(util::trim(std::string("  hello  ")) == "hello");
        expect(util::trimLeft(std::string("  hello")) == "hello");
        expect(util::trimRight(std::string("hello  ")) == "hello");

        // case conversion
        expect(util::toUpper(std::string("hello")) == "HELLO");
        expect(util::toLower(std::string("HELLO")) == "hello");

        // replace
        std::string s = "foo bar foo";
        expect(util::replace(s, std::string("foo"), std::string("baz")) == "baz bar baz");

        // isNumber
        expect(util::isNumber("12345"));
        expect(!util::isNumber("123abc"));
        expect(!util::isNumber(""));

        // endsWith
        expect(util::endsWith("hello.txt", ".txt"));
        expect(!util::endsWith("hello.txt", ".png"));

        // icompare
        expect(util::icompare(std::string("Hello"), std::string("hello")) == 0);
        expect(util::icompare(std::string("abc"), std::string("def")) < 0);

        // randomString
        std::string r1 = util::randomString(16);
        std::string r2 = util::randomString(16);
        expect(r1.size() == 16);
        expect(r2.size() == 16);
        expect(r1 != r2); // extremely unlikely to be equal
    });


    // =========================================================================
    // Util: stream copiers
    //
    describe("util stream copiers", []() {
        std::string data = "hello stream copier world";
        std::istringstream in(data);
        std::string result;
        util::copyToString(in, result);
        expect(result == data);

        // Stream to stream
        std::istringstream in2(data);
        std::ostringstream out;
        util::copyStream(in2, out);
        expect(out.str() == data);
    });


    // =========================================================================
    // Platform (expanded)
    //
    describe("platform expanded", []() {
        expect(!icy::getExePath().empty());
        expect(!icy::getCwd().empty());

        // Hostname should be non-empty
        expect(!icy::getHostname().empty());

        // CPU count should be >= 1
        expect(icy::numCpuCores() >= 1);

        // Memory should be > 0
        expect(icy::getTotalMemory() > 0);
        expect(icy::getFreeMemory() > 0);
        expect(icy::getFreeMemory() <= icy::getTotalMemory());
    });


    // =========================================================================
    // Filesystem (expanded)
    //
    describe("filesystem expanded", []() {
        // filename / basename / extname
        expect(fs::filename("/some/path/file.txt") == "file.txt");
        expect(fs::basename("/some/path/file.txt") == "file");
        expect(fs::extname("/some/path/file.txt") == ".txt");
        expect(fs::extname("/some/path/noext") == "");

        // mkdir / exists / rmdir
        std::string tmpdir = "basetests_tmpdir";
        if (fs::exists(tmpdir))
            fs::rmdir(tmpdir);
        expect(!fs::exists(tmpdir));
        fs::mkdir(tmpdir);
        expect(fs::exists(tmpdir));
        expect(fs::isdir(tmpdir));
        fs::rmdir(tmpdir);
        expect(!fs::exists(tmpdir));

        // savefile / filesize / unlink
        std::string tmpfile = "basetests_tmp.txt";
        std::string content = "test file content";
        fs::savefile(tmpfile, content.c_str(), content.size(), true);
        expect(fs::exists(tmpfile));
        expect(!fs::isdir(tmpfile));
        expect(fs::filesize(tmpfile) == static_cast<int64_t>(content.size()));
        fs::unlink(tmpfile);
        expect(!fs::exists(tmpfile));

        // normalize
        expect(fs::normalize("/some/../other/path") == "/other/path");
    });


    // =========================================================================
    // Thread multiple concurrent
    //
    describe("thread concurrent", []() {
        const int numThreads = 4;
        std::atomic<int> counter{0};
        Thread threads[numThreads];

        for (int i = 0; i < numThreads; ++i) {
            threads[i].start([&counter]() {
                for (int j = 0; j < 1000; ++j)
                    counter++;
            });
        }

        for (int i = 0; i < numThreads; ++i)
            threads[i].join();

        expect(counter == numThreads * 1000);
    });


    // =========================================================================
    // Signal: clear and re-attach
    //
    describe("signal clear and re-attach", []() {
        Signal<void(int&)> sig;
        int value = 0;

        sig += [](int& v) {
            v += 10;
        };
        sig += [](int& v) {
            v += 20;
        };
        sig.emit(value);
        expect(value == 30);

        sig.detachAll();
        expect(sig.nslots() == 0);

        value = 0;
        sig += [](int& v) {
            v += 5;
        };
        sig.emit(value);
        expect(value == 5);
    });


    // =========================================================================
    // Signal: local detach during emit skips later slot
    //
    describe("signal detach during emit skips later slot", []() {
        Signal<void(int)> sig;
        int firstCalls = 0;
        int secondCalls = 0;

        auto secondId = sig.attach([&](int) {
            ++secondCalls;
        });

        auto firstId = sig.attach([&](int) {
            ++firstCalls;
            sig.detach(secondId);
        }, nullptr, -1, 10);
        expect(firstId > secondId);

        sig.emit(0);
        expect(firstCalls == 1);
        expect(secondCalls == 0);
        expect(sig.nslots() == 1);

        sig.emit(1);
        expect(firstCalls == 2);
        expect(secondCalls == 0);
    });


    // =========================================================================
    // Signal: nested thread-safe emit keeps detached slot dead until sweep
    //
    describe("thread signal nested detach keeps slot dead", []() {
        ThreadSignal<void(int)> sig;
        int firstCalls = 0;
        int secondCalls = 0;

        auto secondId = sig.attach([&](int) {
            ++secondCalls;
        });

        auto firstId = sig.attach([&](int depth) {
            ++firstCalls;
            if (depth == 0) {
                sig.detach(secondId);
                sig.emit(1);
            }
        }, nullptr, -1, 10);
        expect(firstId > secondId);

        sig.emit(0);
        expect(firstCalls == 2);
        expect(secondCalls == 0);
        expect(sig.nslots() == 1);

        sig.emit(2);
        expect(firstCalls == 3);
        expect(secondCalls == 0);
    });


    // Define class based tests
    describe("signal", new SignalTest);
    describe("ipc", new IpcTest);
    describe("ipc round-trip", new IpcRoundTripTest);
    describe("timer", new TimerTest);
    describe("timer pause/resume", new TimerPauseResumeTest);
    describe("timer one-shot", new TimerOneShotTest);
    describe("logger filtering", new LoggerFilterTest);
    describe("packet stream", new PacketStreamTest);
    describe("packet stream signal detach", new PacketStreamSignalDetachTest);
    describe("packet stream restart", new PacketStreamRestartTest);
    describe("packet stream active modify guard", new PacketStreamActiveModifyGuardTest);
    describe("packet stream detach cleanup", new PacketStreamDetachCleanupTest);
    describe("packet stream multi-source passthrough", new PacketStreamMultiSourcePassthroughTest);
    describe("process class", new ProcessTest);
    describe("packet stream file io", new PacketStreamIOTest);
    describe("packet stream overflow", new PacketStreamOverflowTest);
    describe("packet stream queue clone boundary", new PacketStreamQueueCloneBoundaryTest);
    describe("packet stream async queue clone boundary", new PacketStreamAsyncQueueCloneBoundaryTest);
    describe("packet stream retention contract", new PacketStreamRetentionContractTest);
    describe("packet stream shared source branch clone boundary", new PacketStreamSharedSourceBranchCloneBoundaryTest);
    // describe("multi packet stream", new MultiPacketStreamTest);

    test::runAll();

    return test::finalize();
}
