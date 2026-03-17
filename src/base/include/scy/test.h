///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/interface.h"
#include <iostream>
#include <list>
#include <map>
#include <mutex>


namespace scy {


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

// Shamelessly define macros to aesthetic name :)
#ifdef NDEBUG
#define expect(x) scy::test::expectImpl(true, "", "", 0)
#else
#define expect(x) scy::test::expectImpl(x, #x, __FILE__, __LINE__)
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


// Test class that runs a static or lambda function.
class Base_API FunctionTest : public Test
{
public:
    std::function<void()> target;

    FunctionTest(std::function<void()> target,
                 const std::string& name = "Unnamed Test")
        : Test(name)
        , target(target)
    {
    }

protected:
    virtual ~FunctionTest() {}

    void run() { target(); }
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

    /// Add a test to the runner.
    void add(Test* test);

    /// Return a pointer to the test matching the given name,
    /// or nullptr if no matching test exists.
    Test* get(const std::string& name) const;

    /// Called by the async context to run the next test.
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
} // namespace scy


/// @\}
