///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//


#pragma once
#include "icy/application.h"
#include "icy/base.h"
#include "icy/base64.h"
#include "icy/buffer.h"
#include "icy/collection.h"
#include "icy/datetime.h"
#include "icy/error.h"
#include "icy/filesystem.h"
#include "icy/idler.h"
#include "icy/ipc.h"
#include "icy/logger.h"
#include "icy/packetio.h"
#include "icy/packetqueue.h"
#include "icy/packetstream.h"
#include "icy/platform.h"
#include "icy/process.h"
#include "icy/random.h"
#include "icy/request.h"
#include "icy/sharedlibrary.h"
#include "icy/signal.h"
#include "icy/stateful.h"
#include "icy/test.h"
#include "icy/thread.h"
#include "icy/time.h"
#include "icy/timer.h"
#include "icy/util.h"

#include <sstream>


using icy::test::Test;
using std::cerr;
using std::cout;


namespace icy {


// =============================================================================
// IPC Test
//
class IpcTest : public Test
{
    int want_x_ipc_callbacks;
    int num_ipc_callbacks;

    void run()
    {
        // std::cout << "Test IPC" << std::endl;

        want_x_ipc_callbacks = 5;
        num_ipc_callbacks = 0;

        ipc::SyncQueue<> ipc;
        ipc.push(new ipc::Action(
            [this](const ipc::Action& a) { ipcCallback(a); }, &ipc, "test1"));
        ipc.push(new ipc::Action(
            [this](const ipc::Action& a) { ipcCallback(a); }, &ipc, "test2"));
        ipc.push(new ipc::Action(
            [this](const ipc::Action& a) { ipcCallback(a); }, &ipc, "test3"));
        ipc.push(new ipc::Action(
            [this](const ipc::Action& a) { ipcCallback(a); }, &ipc, "test4"));
        ipc.push(new ipc::Action(
            [this](const ipc::Action& a) { ipcCallback(a); }, &ipc, "test5"));

        // std::cout << "Test IPC: OK" << std::endl;
        uv::runLoop();

        expect(num_ipc_callbacks == want_x_ipc_callbacks);
    }

    void ipcCallback(const ipc::Action& action)
    {
        // std::cout << "Got IPC callback: " << action.data << std::endl;
        if (++num_ipc_callbacks == want_x_ipc_callbacks)
            reinterpret_cast<ipc::Queue<>*>(action.arg)->close();
    }
};


// =============================================================================
// Timer Test
//
class TimerTest : public Test
{
    void run()
    {
        std::cout << "Starting" << '\n';

        int numTimerTicks = 0;
        int wantTimerTicks = 10;
        bool timerRestarted = false;

        Timer timer(10, 10);
        timer.start([&]() {
            // std::cout << "On timeout: " << timer->count() << std::endl;

            numTimerTicks++;
            if (timer.count() == wantTimerTicks / 2) {
                if (!timerRestarted) {
                    timerRestarted = true;
                    timer.restart(); // restart once, count returns to 0
                } else {
                    timer.handle().unref();
                    timer.stop(); // event loop will be released
                }
            }
        });
        timer.handle().ref();
        // timer.Timeout += [&]() {
        //     // std::cout << "On timeout: " << timer->count() << std::endl;
        //
        //     numTimerTicks++;
        //     if (timer.count() == wantTimerTicks / 2) {
        //         if (!timerRestarted) {
        //             timerRestarted = true;
        //             timer.restart(); // restart once, count returns to 0
        //         } else {
        //             timer.handle().unref();
        //             timer.stop(); // event loop will be released
        //         }
        //     }
        // };

        // std::cout << "Ending" << std::endl;
        uv::runLoop();

        expect(numTimerTicks == wantTimerTicks);
    }
};


// =============================================================================
// Idler Test
//
// class IdlerTest : public Test
// {
//     int wantIdlerTicks;
//     int numIdlerTicks;
//     Idler idler;
//
//     void run()
//     {
//         wantIdlerTicks = 5;
//         numIdlerTicks = 0;
//
//         idler.start(std::bind(&IdlerTest::idlerCallback, this));
//         idler.handle().ref();
//
//         uv::runLoop();
//
//         expect(numIdlerTicks == wantIdlerTicks);
//     }
//
//     void idlerCallback()
//     {
//         // std::cout << "On idle" << std::endl;
//         if (++numIdlerTicks == wantIdlerTicks) {
//             idler.handle().unref();
//             idler.cancel(); // event loop will be released
//         }
//     }
// };


// =============================================================================
// Signal Test
//
// class SignalTest: public Test
// {
//     Signal<void(uint64_t&)> TestSignal;
//
//     void run()
//     {
//         int val = 0;
//         TestSignal += sdelegate(this, &SignalTest::intCallback);
//         TestSignal += slot(this, &SignalTest::intCallbackNoSender);
//         TestSignal.emit(/*this, */val);
//         expect(val == 2);
//
//         val = -1;
//         TestSignal.clear();
//         TestSignal += slot(this, &SignalTest::priorityOne, 1); // last
//         TestSignal += slot(this, &SignalTest::priorityZero, 0); // second
//         TestSignal += slot(this, &SignalTest::priorityMinusOne, -1); // first
//         TestSignal.emit(/*this, */val);
//     }
//
//     void intCallback(void* sender, int& val)
//     {
//         expect(sender == this);
//         val++;
//     }
//
//     void intCallbackNoSender(int& val)
//     {
//         val++;
//     }
//
//     void priorityMinusOne(int& val)
//     {
//         expect(val++ == -1);
//     }
//
//     void priorityZero(int& val)
//     {
//         expect(val++ == 0);
//     }
//
//     void priorityOne(int& val)
//     {
//         expect(val++ == 1);
//     }
// };


struct SignalCounter
{
    void increment(uint64_t& val) { val++; }

    void incrementConst(uint64_t& val) const { val++; }

    static void incrementStatic(uint64_t& val) { val++; }
};


void signalIncrementFree(uint64_t& val)
{
    val++;
}


bool signalHandlerC(const char* sl, size_t ln)
{
    // std::cout << "signalHandlerC: " << sl << ln << std::endl;
    return false;
}


class SignalTest : public Test
{
    struct Foo
    {
        int timesCalled = 0;

        bool signalHandlerA(const char* sl, size_t ln)
        {
            // std::cout << "signalHandlerA: " << sl << ln << std::endl;
            timesCalled++;
            return true;
        }

        bool signalHandlerB(const char* sl, size_t ln) const
        {
            // std::cout << "signalHandlerB: " << sl << ln << std::endl;
            return true;
        }
    };

    void run()
    {
        Foo foo;
        Signal<bool(const char*, size_t)> signal;

        int refid1, refid2, refid3, refid4, refid5;

        // Attach a lambda function
        refid1 = signal.attach([&](const char* arg1, size_t arg2) {
            // std::cout << "lambda: " << arg1 << arg2 << std::endl;

            // Detach slots inside callback scope
            expect(signal.nslots() == 4);
            signal.detach(refid1);
            signal.detach(refid2);
            expect(signal.nslots() == 2);
            return true;
        });
        expect(refid1 == 1);

        // Attach a lambda function via += operator
        refid2 = signal += [&](const char* arg1, size_t arg2) {
            // std::cout << "lambda 2: " << arg1 << arg2 << std::endl;
            return true;
        };
        expect(refid2 == 2);

        // Attach and disconnect a class member slot
        refid3 = signal += slot(&foo, &Foo::signalHandlerA);
        expect(refid3 == 3);
        expect(signal.nslots() == 3);
        bool detached = signal -= slot(&foo, &Foo::signalHandlerA);
        expect(detached == true);
        expect(signal.nslots() == 2);

        // Attach and disconnect a const class member slot
        refid4 = signal += slot(&foo, &Foo::signalHandlerB, 100, -1);
        expect(refid4 == 100);

        // Attach a static function via += operator
        // Auto-ID follows _lastId which was bumped to 100 by explicit slot above
        refid5 = signal += signalHandlerC;
        expect(refid5 == 101);

        signal.emit("the answer to life the universe and everything is", 42);
    }
};


// =============================================================================
// Process
//
class ProcessTest : public Test
{
    void run()
    {
        // Test 1: spawn echo, capture stdout, verify exit code
        {
            bool gotStdout = false;
            bool gotExit = false;
            std::int64_t exitCode = -1;
            std::string output;

            Process proc{"echo", "hello world"};
            proc.onstdout = [&](std::string line) {
                gotStdout = true;
                output += line;
            };
            proc.onexit = [&](std::int64_t status) {
                gotExit = true;
                exitCode = status;
            };
            proc.spawn();

            expect(proc.pid() > 0);

            uv::runLoop();

            expect(gotStdout);
            expect(gotExit);
            expect(exitCode == 0);
            expect(output.find("hello world") != std::string::npos);
        }

        // Test 2: process with multiple args
        {
            std::string output;
            bool gotExit = false;

            Process proc{"echo", "foo", "bar", "baz"};
            proc.onstdout = [&](std::string line) {
                output += line;
            };
            proc.onexit = [&](std::int64_t status) {
                gotExit = true;
                expect(status == 0);
            };
            proc.spawn();
            uv::runLoop();

            expect(gotExit);
            expect(output.find("foo") != std::string::npos);
            expect(output.find("bar") != std::string::npos);
            expect(output.find("baz") != std::string::npos);
        }
    }
};


// =============================================================================
// Packet Stream
//
struct MockThreadedPacketSource : public PacketSource
    , public basic::Startable
{
    Thread runner;
    PacketSignal emitter;

    MockThreadedPacketSource()
        : PacketSource(emitter)
    {
        runner.setRepeating(true);
    }

    void start()
    {
        std::cout << "Start" << '\n';
        runner.start([](void* arg) {
            auto self = reinterpret_cast<MockThreadedPacketSource*>(arg);
            // std::cout << "Emitting" << std::endl;
            RawPacket p("hello", 5);
            self->emitter.emit(/*self, */ p);
            // std::cout << "Emitting 2" << std::endl;
        },
                     this);
    }

    void stop()
    {
        // std::cout << "Stop" << std::endl;
        // runner.close();
        runner.cancel();
        // std::cout << "Stop: OK" << std::endl;
    }
};

struct MockPacketProcessor : public PacketProcessor
{
    PacketSignal emitter;
    int processed = 0;

    MockPacketProcessor()
        : PacketProcessor(emitter)
    {
    }

    void process(IPacket& packet)
    {
        processed++;
        // std::cout << "Process: " << packet.className() << std::endl;
        emitter.emit(packet);
    }
};

struct MockPacketSource : public PacketSource
{
    PacketSignal emitter;

    MockPacketSource()
        : PacketSource(emitter)
    {
    }

    void send(const char* data = "x", size_t len = 1)
    {
        RawPacket packet(data, len);
        emitter.emit(packet);
    }
};

struct MockBurstPacketSource : public PacketSource
{
    Thread runner;
    PacketSignal emitter;
    int packets;

    explicit MockBurstPacketSource(int packets = 1000)
        : PacketSource(emitter)
        , packets(packets)
    {
    }

    void start()
    {
        runner.start([this]() {
            for (int i = 0; i < packets; ++i) {
                RawPacket packet("x", 1);
                emitter.emit(packet);
            }
        });
    }

    void join()
    {
        runner.join();
    }
};

class PacketStreamTest : public Test
{
    int numPackets;

    void onPacketStreamOutput(IPacket& packet)
    {
        // std::cout << "On packet: " << packet.className() << std::endl;
        numPackets++;
    }

    void run()
    {
        numPackets = 0;
        // stream.setRunner(std::make_shared<Thread>());
        PacketStream stream;
        // stream.attach(new AsyncPacketQueue, 0, true);
        stream.attachSource(new MockThreadedPacketSource, true, true);
        // stream.attach(new SyncPacketQueue, 2, true);
        // stream.synchronizeOutput(uv::defaultLoop());
        stream.attach(new MockPacketProcessor, 1, true);

        stream.emitter += slot(this, &PacketStreamTest::onPacketStreamOutput);
        // stream.emitter += packetSlot(this,
        // &PacketStreamTest::onPacketStreamOutput);
        // stream.emitter.attach<PacketStreamTest,
        // &PacketStreamTest::onPacketStreamOutput>(this);

        stream.start();

        // TODO: Test pause/resume functionality
        // Run the thread for 100ms
        icy::sleep(100);

        stream.close();

        expect(numPackets > 0);
    }
};

class PacketStreamSignalDetachTest : public Test
{
    void run()
    {
        PacketSignal source;
        PacketStream stream;

        stream.attachSource(source);
        expect(stream.numSources() == 1);

        expect(stream.detachSource(source));
        expect(stream.numSources() == 0);
        expect(!stream.detachSource(source));
    }
};

class PacketStreamRestartTest : public Test
{
    int numPackets = 0;

    void onPacketStreamOutput(IPacket&)
    {
        numPackets++;
    }

    void run()
    {
        PacketStream stream;
        MockPacketSource source;
        MockPacketProcessor processor;

        stream.attachSource(&source, false, false);
        stream.attach(&processor, 1, false);
        stream.emitter += slot(this, &PacketStreamRestartTest::onPacketStreamOutput);

        stream.start();
        expect(source.emitter.nslots() == 1);
        expect(processor.emitter.nslots() == 1);

        source.send("a", 1);
        expect(numPackets == 1);
        expect(processor.processed == 1);

        stream.stop();
        expect(source.emitter.nslots() == 0);
        expect(processor.emitter.nslots() == 0);

        source.send("b", 1);
        expect(numPackets == 1);
        expect(processor.processed == 1);

        stream.start();
        expect(source.emitter.nslots() == 1);
        expect(processor.emitter.nslots() == 1);

        source.send("c", 1);
        expect(numPackets == 2);
        expect(processor.processed == 2);

        stream.close();
        expect(source.emitter.nslots() == 0);
        expect(processor.emitter.nslots() == 0);
    }
};

class PacketStreamMultiSourcePassthroughTest : public Test
{
    std::atomic<int> received{0};

    void onPacketStreamOutput(IPacket&)
    {
        ++received;
    }

    void run()
    {
        PacketStream stream;
        auto sourceA = std::make_shared<MockBurstPacketSource>(1000);
        auto sourceB = std::make_shared<MockBurstPacketSource>(1000);

        stream.attachSource(sourceA, false);
        stream.attachSource(sourceB, false);
        stream.emitter += slot(this, &PacketStreamMultiSourcePassthroughTest::onPacketStreamOutput);

        stream.start();

        sourceA->start();
        sourceB->start();
        sourceA->join();
        sourceB->join();

        expect(received == 2000);

        stream.close();
    }
};

static std::string RANDOM_CONTENT = "r@ndom";

struct PacketStreamIOTest : public Test
{
    int numPackets;

    PacketStreamIOTest()
    {
        fs::savefile("input.txt", RANDOM_CONTENT.c_str(),
                     RANDOM_CONTENT.length(), true);
    }

    ~PacketStreamIOTest()
    {
        fs::unlink("input.txt");
        fs::unlink("output.txt");
    }

    void run()
    {
        numPackets = 0;
        PacketStream stream;
        stream.attachSource(new ThreadedStreamReader(new std::ifstream("input.txt")), true, true);
        stream.attach(new StreamWriter(new std::ofstream("output.txt")), 1, true);
        stream.start();
        // Run the thread for 100ms
        icy::sleep(100);

        stream.close();

        // Verify result
        std::string result;
        std::ifstream ofile("output.txt");
        util::copyToString(ofile, result);
        expect(result == RANDOM_CONTENT);
    }
};

class MultiPacketStreamTest : public Test
{
    void onChildPacketStreamOutput(void* sender, IPacket& packet)
    {
        // std::cout << "On child packet: " << packet.className() << std::endl;
    }

    struct ChildStreams
    {
        PacketStream* s1;
        PacketStream* s2;
        PacketStream* s3;
    };

    void run()
    {
        /// //stream.setRunner(std::make_shared<Thread>());
        // stream.attachSource(new MockThreadedPacketSource, true, true);
        // //stream.attach(new AsyncPacketQueue, 0, true);
        // stream.attach(new MockPacketProcessor, 1, true);
        // //stream.emitter += packetSlot(this, &Tests::onPacketStreamOutput);
        // stream.start();
        ///
        /// // The second PacketStream receives packets from the first one
        /// // and synchronizes output packets with the default event loop.
        /// ChildStreams children;
        // children.s1 = new PacketStream;
        // //children.s1->setRunner(std::make_shared<Idler>()); // Use Idler
        /// children.s1->attachSource(stream.emitter);
        // children.s1->attach(new AsyncPacketQueue, 0, true);
        // children.s1->attach(new SyncPacketQueue, 1, true);
        // children.s1->synchronizeOutput(uv::defaultLoop());
        // children.s1->emitter += packetSlot(this,
        // &MultiPacketStreamTest::onChildPacketStreamOutput);
        // children.s1->start();
        ///
        /// children.s2 = new PacketStream;
        // children.s2->attachSource(stream.emitter);
        // children.s2->attach(new AsyncPacketQueue, 0, true);
        // children.s2->synchronizeOutput(uv::defaultLoop());
        // children.s2->emitter += packetSlot(this,
        // &MultiPacketStreamTest::onChildPacketStreamOutput);
        // children.s2->start();
        ///
        /// children.s3 = new PacketStream;
        // children.s3->attachSource(stream.emitter);
        // children.s3->attach(new AsyncPacketQueue, 0, true);
        // children.s3->synchronizeOutput(uv::defaultLoop());
        // children.s3->emitter += packetSlot(this,
        // &MultiPacketStreamTest::onChildPacketStreamOutput);
        // children.s3->start();
        ///
        /// // app.waitForShutdown([](void* arg) {
        /// //     auto streams = reinterpret_cast<ChildStreams*>(arg);
        // //     //streams->s1->attachSource(stream.emitter);
        // //     if (streams->s1) delete streams->s1;
        // //     if (streams->s2) delete streams->s2;
        // //     if (streams->s3) delete streams->s3;
        // // }, &children);
        ///
        /// uv::runLoop();
        ///
        /// if (children.s1) delete children.s1;
        // if (children.s2) delete children.s2;
        // if (children.s3) delete children.s3;
        // PacketStream stream;
    }
};


// =============================================================================
// Timer Pause/Resume Test
//
class TimerPauseResumeTest : public Test
{
    void run()
    {
        // Test: timer fires, we stop it, restart it, verify total ticks
        int ticksBeforeStop = 0;
        int ticksAfterRestart = 0;
        bool stopped = false;
        bool restarted = false;

        Timer timer(5, 5);
        Timer restartTimer(20);

        timer.start([&]() {
            if (!stopped) {
                ticksBeforeStop++;
                if (ticksBeforeStop == 3) {
                    stopped = true;
                    timer.stop();
                    // Restart after a brief delay
                    restartTimer.start([&]() {
                        restarted = true;
                        restartTimer.handle().unref();
                        timer.start();
                    });
                    restartTimer.handle().ref();
                }
            } else {
                ticksAfterRestart++;
                if (ticksAfterRestart == 3) {
                    timer.handle().unref();
                    timer.stop();
                }
            }
        });
        timer.handle().ref();
        uv::runLoop();

        expect(ticksBeforeStop == 3);
        expect(ticksAfterRestart == 3);
        expect(restarted == true);
    }
};


// =============================================================================
// Timer One-shot Test
//
class TimerOneShotTest : public Test
{
    void run()
    {
        int ticks = 0;

        // One-shot timer: timeout set, no interval (0)
        Timer timer(std::int64_t(10), std::int64_t(0));
        timer.start([&]() {
            ticks++;
            timer.handle().unref();
        });
        timer.handle().ref();
        uv::runLoop();

        expect(ticks == 1);
    }
};


// =============================================================================
// IPC Round-trip Test
//
class IpcRoundTripTest : public Test
{
    void run()
    {
        // Test: push actions from a worker thread, verify they execute
        // on the event loop thread
        std::atomic<int> callbackCount{0};
        std::vector<std::string> receivedData;
        std::thread::id loopThreadId = std::this_thread::get_id();
        bool allOnLoopThread = true;

        ipc::SyncQueue<> ipc;

        // Push from a background thread
        Thread worker;
        worker.start([&]() {
            for (int i = 0; i < 3; i++) {
                ipc.push(new ipc::Action(
                    [&](const ipc::Action& action) {
                        if (std::this_thread::get_id() != loopThreadId)
                            allOnLoopThread = false;
                        receivedData.push_back(action.data);
                        if (++callbackCount == 3) {
                            // Stop the event loop from the callback
                            uv::stopLoop();
                        }
                    },
                    &ipc, "msg" + std::to_string(i)));
                icy::sleep(5);
            }
        });

        uv::runLoop();
        ipc.close();
        // Run once more to process the close
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        worker.join();

        expect(callbackCount == 3);
        expect(receivedData.size() == 3);
        expect(receivedData[0] == "msg0");
        expect(receivedData[1] == "msg1");
        expect(receivedData[2] == "msg2");
        expect(allOnLoopThread == true);
    }
};


// =============================================================================
// Logger Level Filtering Test
//
class LoggerFilterTest : public Test
{
    void run()
    {
#ifdef ICY_ENABLE_LOGGING
        // Custom channel that captures messages
        struct CaptureChannel : public LogChannel
        {
            std::vector<std::string> messages;
            std::vector<Level> levels;

            CaptureChannel(const std::string& name, Level level)
                : LogChannel(name, level)
            {
            }

            void write(const LogStream& stream) override
            {
                if (this->level() > stream.level)
                    return;
                messages.push_back(stream.message.str());
                levels.push_back(stream.level);
            }
        };

        // Test 1: channel at Warn level should filter out Trace/Debug/Info
        {
            auto channel = std::make_unique<CaptureChannel>("filter_test", Level::Warn);
            auto* channelPtr = channel.get();
            Logger::instance().add(std::move(channel));

            // These should be filtered out (below Warn threshold)
            LogStream(Level::Trace, "test.cpp", 1, "filter_test").write("trace msg");
            LogStream(Level::Debug, "test.cpp", 2, "filter_test").write("debug msg");
            LogStream(Level::Info, "test.cpp", 3, "filter_test").write("info msg");

            // These should pass through
            LogStream(Level::Warn, "test.cpp", 4, "filter_test").write("warn msg");
            LogStream(Level::Error, "test.cpp", 5, "filter_test").write("error msg");

            expect(channelPtr->messages.size() == 2);
            expect(channelPtr->levels[0] == Level::Warn);
            expect(channelPtr->levels[1] == Level::Error);

            Logger::instance().remove("filter_test");
        }

        // Test 2: channel at Trace level should pass everything
        {
            auto channel = std::make_unique<CaptureChannel>("pass_all", Level::Trace);
            auto* channelPtr = channel.get();
            Logger::instance().add(std::move(channel));

            LogStream(Level::Trace, "test.cpp", 1, "pass_all").write("a");
            LogStream(Level::Debug, "test.cpp", 2, "pass_all").write("b");
            LogStream(Level::Info, "test.cpp", 3, "pass_all").write("c");
            LogStream(Level::Warn, "test.cpp", 4, "pass_all").write("d");
            LogStream(Level::Error, "test.cpp", 5, "pass_all").write("e");

            expect(channelPtr->messages.size() == 5);

            Logger::instance().remove("pass_all");
        }
#endif // ICY_ENABLE_LOGGING
    }
};


// =============================================================================
// Packet Stream Overflow Test
//
// Verifies that a PacketStream with a bounded SyncPacketQueue handles a fast
// producer gracefully: no crash, packets are purged when the queue limit is
// exceeded, and the consumer receives some subset of sent packets.
//
class PacketStreamOverflowTest : public Test
{
    std::atomic<int> numReceived{0};

    void onPacket(IPacket& packet)
    {
        numReceived++;
    }

    void run()
    {
        const int totalToSend = 500;
        const int queueLimit = 10; // small queue to force overflow/purging

        PacketStream stream;
        auto queue = std::make_shared<SyncPacketQueue<>>(uv::defaultLoop(), queueLimit);

        // Attach a SyncPacketQueue with a small capacity so the queue's
        // push() will purge old packets when the limit is reached.
        stream.attach(queue, 0);
        stream.attach(new MockPacketProcessor, 1, true);
        stream.emitter += slot(this, &PacketStreamOverflowTest::onPacket);

        stream.start();

        // Blast packets from a background thread as fast as possible
        Thread producer;
        producer.start([&]() {
            for (int i = 0; i < totalToSend; i++) {
                stream.write(RawPacket("overflow", 8));
            }
        });

        // Give the event loop time to drain some packets
        icy::sleep(200);

        // Run the event loop briefly to dispatch queued items
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);
        icy::sleep(50);
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);

        producer.join();
        stream.close();

        // The key assertions:
        // 1. We didn't crash (reaching here proves it)
        // 2. The bounded queue purged some packets under load
        // 3. We received some packets but fewer than sent (due to purging)
        expect(queue->dropped() > 0);
        expect(queue->dropped() < static_cast<size_t>(totalToSend));
        expect(numReceived > 0);
        expect(numReceived <= totalToSend);

        std::cout << "PacketStreamOverflow: sent=" << totalToSend
                  << " dropped=" << queue->dropped()
                  << " received=" << numReceived << '\n';
    }
};


} // namespace icy


/// @\}
