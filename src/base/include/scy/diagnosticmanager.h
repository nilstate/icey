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


#include "scy/collection.h"
#include "scy/stateful.h"
#include "scy/thread.h"

#include <stdexcept>


namespace scy {


struct DiagnosticState : public State
{
    enum Type
    {
        None = 0,
        Checking,
        Passed,
        Failed
    };

    std::string str(unsigned int id) const
    {
        switch (id) {
            case None:
                return "None";
            case Checking:
                return "Checking";
            case Passed:
                return "Passed";
            case Failed:
                return "Failed";
            default:
                throw std::logic_error("Unknown DiagnosticState id: " + std::to_string(id));
        }
    }
};


//
// Diagnostic Interface
//


class /* SCY_EXTERN */ IDiagnostic : public Stateful<DiagnosticState>
{
public:
    IDiagnostic();
    virtual ~IDiagnostic();

    std::string name;                 ///< The name of the diagnostic.
    std::string description;          ///< The diagnostic description.
    std::vector<std::string> summary; ///< The diagnostic summary, maybe including
                                      ///< troubleshooting information on failure.

    virtual void check();
    virtual void reset();

    virtual bool complete() const;
    virtual bool passed() const;
    virtual bool failed() const;

    /// Signals when a new text item is added
    /// to the summary.
    Signal<void(const std::string&)> SummaryUpdated;

    /// The StateChange signal will dispatch
    /// diagnostic test results to delegates.

protected:
    /// Override to implement diagnostic logic.
    virtual void run() = 0;

    virtual bool pass();
    virtual bool fail();
    virtual void addSummary(const std::string& text);
};


using DiagnosticStore = PointerCollection<std::string, IDiagnostic>;


//
// Asynchronous Diagnostic Base
//


class /* SCY_EXTERN */ AsyncDiagnostic : public IDiagnostic
    , public basic::Runnable
{
public:
    virtual ~AsyncDiagnostic() {};

    virtual void run() = 0;

    virtual void check() override
    {
        reset();
        _thread.start(std::bind(&AsyncDiagnostic::run, this));
    };

protected:
    Thread _thread;
};


//
// Diagnostic Manager
//


class /* SCY_EXTERN */ DiagnosticManager : public DiagnosticStore
{
public:
    DiagnosticManager();
    virtual ~DiagnosticManager();

    bool freeDiagnostic(const std::string& name);

    /// Adds a IDiagnostic test instance.
    bool addDiagnostic(IDiagnostic* test);

    /// Returns the IDiagnostic instance or throws
    /// a NotFoundException exception.
    virtual IDiagnostic* getDiagnostic(const std::string& name);

    virtual void resetAll();

    /// Runs all managed IDiagnostic tests.
    /// DiagnosticsComplete will be dispatched on
    /// completion.
    virtual void checkAll();

    virtual bool allComplete();

    NullSignal DiagnosticsComplete;

    virtual void onDiagnosticStateChange(void*, DiagnosticState& state, const DiagnosticState&);
};


} // namespace scy


/// @\}
