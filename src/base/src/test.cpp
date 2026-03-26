///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/test.h"
#include "icy/logger.h"
#include "icy/loop.h"
#include "icy/singleton.h"
#include "icy/time.h"
#include "icy/util.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>


using std::cerr;
using std::cout;


namespace icy {
namespace test {


static Singleton<TestRunner> singleton;


void init()
{
    // Set the logger to only log warning level and above if no debug
    // channel has been set yet.
    if (!Logger::instance().get("debug", false))
        Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Warn));

    // Initialize the default test runner.
    TestRunner::getDefault();

    // Nothing else to do...
}


int finalize()
{
    bool passed = TestRunner::getDefault().passed();
    singleton.destroy();

    return passed ? 0 : 1;
}


void runAll()
{
    TestRunner::getDefault().run();
}


void describe(const std::string& name, std::function<void()> target)
{
    auto test = new FunctionTest(target, name);
    TestRunner::getDefault().add(test);
}


void describe(const std::string& name, Test* test)
{
    test->name = name;
    TestRunner::getDefault().add(test);
}


void expectImpl(bool passed, const char* assert, const char* file, long line)
{
    if (passed)
        return;

    std::stringstream ss;
    ss << "failed on " << assert << " in " << file << " at " << line;

    auto test = TestRunner::getDefault().current();
    if (test) {
        test->errors.push_back(ss.str());
    }

    std::cout << ss.str() << '\n';
}


bool waitFor(std::function<bool()> condition, int timeoutMs)
{
    if (condition())
        return true;

    auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(timeoutMs);

    while (!condition()) {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;

        // Process pending libuv events without blocking.
        // UV_RUN_NOWAIT returns immediately if no callbacks are pending,
        // so we sleep briefly to avoid busy-waiting.
        uv::runLoop(uv::defaultLoop(), UV_RUN_NOWAIT);

        if (!condition()) {
            // Yield to allow I/O to complete
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    return true;
}


//
// Test Runner
//


TestRunner::TestRunner()
    : _current(nullptr)
{
}


TestRunner::~TestRunner()
{
    clear();
}


void TestRunner::add(Test* test)
{
    cout << test->name << " added" << '\n';
    std::lock_guard<std::mutex> guard(_mutex);
    _tests.push_back(test);
}


Test* TestRunner::get(std::string_view name) const
{
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto* test : _tests) {
        if (test->name == name)
            return test;
    }
    return nullptr;
}


void TestRunner::clear()
{
    std::lock_guard<std::mutex> guard(_mutex);
    util::clearList<Test>(_tests);
}


TestList TestRunner::tests() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _tests;
}


ErrorMap TestRunner::errors() const
{
    ErrorMap errors;
    TestList tests = this->tests();
    for (auto* test : tests) {
        if (!test->passed()) {
            errors[test] = test->errors;
        }
    }
    return errors;
}


bool TestRunner::passed() const
{
    return errors().empty();
}


Test* TestRunner::current() const
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _current;
}


void TestRunner::run()
{
    cout << "===============================================================" << '\n';
    // cout << "running all tests" << endl;

    uint64_t start = time::hrtime();
    double duration = 0;
    TestList tests = this->tests();
    for (auto* test : tests) {
        {
            std::lock_guard<std::mutex> guard(_mutex);
            _current = test;
        }
        cout
            << "---------------------------------------------------------------" << '\n';
        cout << _current->name << " starting" << '\n';
        uint64_t test_start = time::hrtime();
        try {
            _current->run();
        } catch (std::exception& exc) {
            _current->errors.push_back(exc.what());
            cout << "exception thrown: " << exc.what() << '\n';
        }
        _current->duration = (time::hrtime() - test_start) / 1e9;
        cout << _current->name << " ended after " << _current->duration << " seconds" << '\n';
    }

    duration = (time::hrtime() - start) / 1e9;

    cout << "---------------------------------------------------------------" << '\n';
    cout << "all tests completed after " << duration << " seconds" << '\n';
    // cout << "summary: " << endl;

    for (auto* test : tests) {
        if (test->passed()) {
            cout << test->name << " passed" << '\n';
        } else {
            cout << test->name << " failed" << '\n';
        }
    }
}


TestRunner& TestRunner::getDefault()
{
    return *singleton.get();
}


//
// Test
//


Test::Test(const std::string& name)
    : name(name)
    , duration(0)
{
}


Test::~Test()
{
    // cout << "destroying " << name << endl;
}


bool Test::passed()
{
    return errors.empty();
}


} // namespace test
} // namespace icy


/// @}
