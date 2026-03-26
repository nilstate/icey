///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/interface.h"
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <string_view>


namespace icy {


/// Modern unit testing framework.
namespace test {


class Base_API Test;
class Base_API TestRunner;

using TestList = std::list<Test*>;
using SErrorist = std::list<std::string>;
using ErrorMap = std::map<Test*, SErrorist>;

/// Initialize the test environment.
Base_API void init();

/// Finalize the test environment.
///
/// Destroy the TestRunner singleton instance and return the exit code.
Base_API int finalize();

/// Run all tests.
Base_API void runAll();

/// Describe a test environment implemented by the given lambda function.
Base_API void describe(const std::string& name, std::function<void()> target);

/// Describe a test environment implemented by the given test instance.
Base_API void describe(const std::string& name, Test* test);

/// Expect asserts that a condition is true (use expect() as defined below).
Base_API void expectImpl(bool passed, const char* assert, const char* file, long line);

/// Run the event loop until a condition is met or timeout expires.
/// Returns true if the condition was satisfied, false on timeout.
/// Useful for testing async operations that complete via libuv callbacks.
Base_API bool waitFor(std::function<bool()> condition, int timeoutMs = 3000);

// Shamelessly define macros to aesthetic name :)
#ifdef NDEBUG
#define expect(x) ((void)(x), icy::test::expectImpl(true, "", "", 0))
#else
#define expect(x) icy::test::expectImpl(x, #x, __FILE__, __LINE__)
#endif


//
// Test
//


/// Test wrapper class.
///
/// This class is for implementing any kind of unit
/// test that can be executed by a `TestRunner`.
///
class Base_API Test
{
public:
    /// @param name Human-readable name displayed in test output.
    Test(const std::string& name = "Unnamed Test");

    /// Should remain protected.
    virtual ~Test();

    /// Called by the TestRunner to run the test.
    virtual void run() = 0;

    /// Return true when the test passed without errors.
    bool passed();

    /// The name of the test.
    std::string name;

    /// A list of test errors.
    SErrorist errors;

    /// The test run duration for benchmarking.
    double duration;

protected:
    Test(const Test& test) = delete;
    Test& operator=(Test const&) = delete;

    /// Tests belong to a TestRunner instance.
    friend class TestRunner;
};


/// Test wrapper for standalone test functions
class Base_API FunctionTest : public Test
{
public:
    std::function<void()> target;

    /// @param target Lambda or function to execute as the test body.
    /// @param name   Human-readable test name.
    FunctionTest(std::function<void()> target,
                 const std::string& name = "Unnamed Test")
        : Test(name)
        , target(target)
    {
    }

protected:
    virtual ~FunctionTest() {}

    void run() override { target(); }
};


//
// Test Runner
//


/// Test manager queue.
///
/// The `TestRunner` is a queue in charge of running one or many tests.
///
/// When `run()` the `TestRunner` loops through each test in the list calling
/// the test's `run()` method.
///
class Base_API TestRunner
{
public:
    TestRunner();
    virtual ~TestRunner();

    /// Adds a test to the runner and prints its name to stdout.
    /// @param test Non-null pointer to the test; the runner takes ownership.
    void add(Test* test);

    /// Return a pointer to the test matching the given name,
    /// or nullptr if no matching test exists.
    /// @param name Test name to search for.
    /// @return Matching test pointer or nullptr.
    Test* get(std::string_view name) const;

    /// Runs all registered tests sequentially, printing results to stdout.
    void run();

    /// Destroy and clears all managed tests.
    void clear();

    /// Return the currently active Test or nullptr.
    Test* current() const;

    /// Return the list of tests.
    TestList tests() const;

    /// Return a map of tests and errors.
    ErrorMap errors() const;

    /// Return true if all tests passed.
    bool passed() const;

    /// Return the default `TestRunner` singleton, although
    /// `TestRunner` instances may also be initialized individually.
    static TestRunner& getDefault();

protected:
    mutable std::mutex _mutex;
    TestList _tests;
    Test* _current;
};


} // namespace test
} // namespace icy


/// @}
