///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "icy/diagnosticmanager.h"
#include "icy/logger.h"


namespace icy {


IDiagnostic::IDiagnostic()
{
}


IDiagnostic::~IDiagnostic()
{
}


void IDiagnostic::reset()
{
    summary.clear();
    setState(this, DiagnosticState::None);
}


void IDiagnostic::check()
{
    reset();
    run();
}


void IDiagnostic::addSummary(const std::string& text)
{
    summary.push_back(text);
    SummaryUpdated.emit(text);
}


bool IDiagnostic::pass()
{
    return setState(this, DiagnosticState::Passed);
}


bool IDiagnostic::fail()
{
    return setState(this, DiagnosticState::Failed);
}


bool IDiagnostic::complete() const
{
    return stateEquals(DiagnosticState::Passed) ||
           stateEquals(DiagnosticState::Failed);
}


bool IDiagnostic::passed() const
{
    return stateEquals(DiagnosticState::Passed);
}


bool IDiagnostic::failed() const
{
    return stateEquals(DiagnosticState::Failed);
}


// ---------------------------------------------------------------------
//
DiagnosticManager::DiagnosticManager()
{
    LTrace("Create");
}


DiagnosticManager::~DiagnosticManager()
{
    LTrace("Destroy");
}


bool DiagnosticManager::addDiagnostic(std::unique_ptr<IDiagnostic> test)
{
    if (!test)
        throw std::invalid_argument("DiagnosticManager: test cannot be null");
    if (test->name.empty())
        throw std::invalid_argument("DiagnosticManager: test name cannot be empty");

    LTrace("Adding Diagnostic: ", test->name);
    return tryAdd(test->name, std::move(test));
}


bool DiagnosticManager::freeDiagnostic(const std::string& name)
{
    if (name.empty())
        throw std::invalid_argument("DiagnosticManager: name cannot be empty");

    LTrace("Removing Diagnostic: ", name);
    return erase(name);
}


IDiagnostic* DiagnosticManager::getDiagnostic(const std::string& name) const
{
    return get(name);
}


void DiagnosticManager::resetAll()
{
    for (auto& [name, test] : _map) {
        test->reset();
    }
}


void DiagnosticManager::checkAll()
{
    for (auto& [name, test] : _map) {
        test->check();
    }
}


bool DiagnosticManager::allComplete() const
{
    for (auto& [name, test] : _map) {
        if (!test->complete())
            return false;
    }
    return true;
}


void DiagnosticManager::onDiagnosticStateChange(void* sender,
                                                DiagnosticState& state,
                                                const DiagnosticState&)
{
    auto test = reinterpret_cast<IDiagnostic*>(sender);
    STrace << "Diagnostic state change: " << test->name << ": " << state;

    if (test->complete() && allComplete())
        DiagnosticsComplete.emit();
}


} // namespace icy

/// @}
