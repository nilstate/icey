///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup json
/// @{


#include "scy/json/configuration.h"
#include "scy/logger.h"


using std::endl;


namespace scy {
namespace json {


Configuration::Configuration()
    : _loaded(false)
{
}


Configuration::~Configuration()
{
}


void Configuration::load(const std::string& path, bool create)
{
    _path = path;
    load(create);
}


void Configuration::load(bool /* create */)
{
    std::lock_guard<std::mutex> guard(_mutex);

    if (_path.empty())
        throw std::runtime_error(
            "Cannot load configuration: File path not set.");

    LDebug("Load: ", _path);

    try {
        // if (create && !fs::exists(_path))
        //    fs::createFile(_path);

        json::loadFile(_path, root);
    } catch (...) {
        // The config file may be empty,
        // but the path is set so we can save.
    }

    _loaded = true;
}


void Configuration::save()
{
    std::lock_guard<std::mutex> guard(_mutex);

    if (_path.empty())
        throw std::runtime_error(
            "Cannot save: Configuration file path must be set.");

    // Will throw on error
    json::saveFile(_path, root);
}


std::string Configuration::path()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _path;
}


bool Configuration::loaded()
{
    std::lock_guard<std::mutex> guard(_mutex);
    return _loaded;
}


void Configuration::print(std::ostream& ost)
{
    ost << root.dump(4);
}


bool Configuration::remove(const std::string& key)
{
    std::lock_guard<std::mutex> guard(_mutex);

    return !!root.erase(key);

    // return root.removeMember(key) != Json::nullValue;
}


void Configuration::removeAll(const std::string& baseKey)
{
    std::lock_guard<std::mutex> guard(_mutex);

    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key().find(baseKey) != std::string::npos)
            it = it->erase(it);
    }

    //json::value::Members members = root.getMemberNames();
    //for (unsigned i = 0; i < members.size(); i++) {
    //    if (members[i].find(baseKey) != std::string::npos)
    //        root.removeMember(members[i]);
    //}
}


void Configuration::replace(const std::string& from, const std::string& to)
{
    std::lock_guard<std::mutex> guard(_mutex);

    std::string data = root.dump();
    util::replaceInPlace(data, from, to);

    root = json::value::parse(data.begin(), data.end());

    //std::stringstream ss;
    //ss << data;
    //root = json::value::parse(ss); // cannot parse a string?
}


bool Configuration::getRaw(const std::string& key, std::string& value) const
{
    std::lock_guard<std::mutex> guard(_mutex);

    if (root.find(key) != root.end()) {
        value = root[key].get<std::string>();
        return true;
    }
    return false;
}


void Configuration::setRaw(const std::string& key, const std::string& value)
{
    {
        std::lock_guard<std::mutex> guard(_mutex);
        (root)[key] = value;
    }
    PropertyChanged.emit(key, value);
}


void Configuration::keys(std::vector<std::string>& keys, const std::string& baseKey)
{
    std::lock_guard<std::mutex> guard(_mutex);

    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key().find(baseKey) != std::string::npos)
            keys.push_back(it.key());
    }

    //json::value::Members members = root.getMemberNames();
    //for (unsigned i = 0; i < members.size(); i++) {
    //    if (members[i].find(baseKey) != std::string::npos)
    //        keys.push_back(members[i]);
    //}
}


} // namespace json
} // namespace scy


/// @\}
