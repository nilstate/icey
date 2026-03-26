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


#include "icy/collection.h"
#include "icy/stateful.h"
#include "icy/thread.h"

#include <stdexcept>


namespace icy {


/// State definitions for diagnostic providers
struct DiagnosticState : public State
{
    enum Type
    {
        None = 0,
        Checking,
        Passed,
        Failed
    };

    /// Returns a human-readable string for the given state ID.
    /// @param id A DiagnosticState::Type value.
    /// @return State name string.
    /// @throws std::logic_error for unknown IDs.
    std::string str(unsigned int id) const override
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


/// Abstract interface for diagnostic information providers
class /* ICY_EXTERN */ IDiagnostic : public Stateful<DiagnosticState>
{
public:
    IDiagnostic();
    virtual ~IDiagnostic();

    std::string name;                 ///< The name of the diagnostic.
    std::string description;          ///< The diagnostic description.
    std::vector<std::string> summary; ///< The diagnostic summary, maybe including
                                      ///< troubleshooting information on failure.

    /// Resets state to None and invokes run() to perform the diagnostic check.
    virtual void check();

    /// Clears the summary and resets state to None.
    virtual void reset();

    /// Returns true if the diagnostic has reached a terminal state (Passed or Failed).
    /// @return true if complete.
    virtual bool complete() const;

    /// Returns true if the diagnostic state is Passed.
    /// @return true if passed.
    virtual bool passed() const;

    /// Returns true if the diagnostic state is Failed.
    /// @return true if failed.
    virtual bool failed() const;

    /// Signals when a new text item is added
    /// to the summary.
    ThreadSignal<void(const std::string&)> SummaryUpdated;

    /// The StateChange signal will dispatch
    /// diagnostic test results to delegates.

protected:
    /// Override to implement diagnostic logic.
    virtual void run() = 0;

    /// Transitions the state to Passed.
    /// @return true if the state transition succeeded.
    virtual bool pass();

    /// Transitions the state to Failed.
    /// @return true if the state transition succeeded.
    virtual bool fail();

    /// Appends text to the summary list and emits SummaryUpdated.
    /// @param text Summary line to append.
    virtual void addSummary(const std::string& text);
};


//
// Asynchronous Diagnostic Base
//


/// Asynchronous diagnostic information collector
class /* ICY_EXTERN */ AsyncDiagnostic : public IDiagnostic
    , public basic::Runnable
{
public:
    virtual ~AsyncDiagnostic() {};

    void run() override = 0;

    /// Resets the diagnostic and launches run() on a background thread.
    void check() override
    {
        reset();
        _thread.start([this]() { run(); });
    };

protected:
    Thread _thread;
};


//
// Diagnostic Manager
//


/// Registry and manager for diagnostic providers
class /* ICY_EXTERN */ DiagnosticManager : public KeyedStore<std::string, IDiagnostic>
{
public:
    DiagnosticManager();
    ~DiagnosticManager();

    /// Adds a diagnostic, taking ownership.
    bool addDiagnostic(std::unique_ptr<IDiagnostic> test);

    /// Removes and deletes the diagnostic registered under name.
    bool freeDiagnostic(const std::string& name);

    /// Returns the diagnostic or nullptr.
    IDiagnostic* getDiagnostic(const std::string& name) const;

    void resetAll();
    void checkAll();
    bool allComplete() const;

    NullSignal DiagnosticsComplete;

    void onDiagnosticStateChange(void*, DiagnosticState& state, const DiagnosticState&);
};


} // namespace icy


/// @}
