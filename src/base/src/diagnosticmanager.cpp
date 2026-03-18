///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#include "scy/diagnosticmanager.h"

#include <memory>



namespace scy {


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

void DiagnosticManager::resetAll()
{
    std::unique_lock<std::shared_mutex> guard(_mutex);
    for (auto& [name, test] : _map) {
        test->reset();
    }
}


void DiagnosticManager::checkAll()
{
    std::unique_lock<std::shared_mutex> guard(_mutex);
    for (auto& [name, test] : _map) {
        test->check();
    }
}


bool DiagnosticManager::allComplete()
{
    std::shared_lock<std::shared_mutex> guard(_mutex);
    for (auto& [name, test] : _map) {
        if (!test->complete())
            return false;
    }
    return true;
}


bool DiagnosticManager::addDiagnostic(IDiagnostic* test)
{
    if (!test)
        throw std::invalid_argument("DiagnosticManager: test cannot be null");
    if (test->name.empty())
        throw std::invalid_argument("DiagnosticManager: test name cannot be empty");

    LTrace("Adding Diagnostic: ", test->name);
    // test->StateChange += sdelegate(this,
    // &DiagnosticManager::onDiagnosticStateChange);
    return DiagnosticStore::add(test->name, test);
}


bool DiagnosticManager::freeDiagnostic(const std::string& name)
{
    if (name.empty())
        throw std::invalid_argument("DiagnosticManager: name cannot be empty");

    LTrace("Removing Diagnostic: ", name);
    std::unique_ptr<IDiagnostic>
        test(DiagnosticStore::remove(name));
    if (test) {
        return true;
    }
    return false;
}


IDiagnostic* DiagnosticManager::getDiagnostic(const std::string& name)
{
    return DiagnosticStore::get(name, true);
}


void DiagnosticManager::onDiagnosticStateChange(void* sender,
                                                DiagnosticState& state,
                                                const DiagnosticState&)
{
    auto test = reinterpret_cast<IDiagnostic*>(sender);
    STrace << "Diagnostic state change: " << test->name << ": " << state;

    if (test->complete() && allComplete())
        DiagnosticsComplete.emit(/*this*/);
}


} // namespace scy
