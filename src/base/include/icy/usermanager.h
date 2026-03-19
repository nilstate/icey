///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/collection.h"
#include <map>


namespace icy {


struct IUser
{
    virtual std::string username() const = 0;
    virtual std::string password() const = 0;
};


class /* ICY_EXTERN */ BasicUser : public IUser
{
public:
    BasicUser(const std::string& username, const std::string& password = "")
        : _username(username)
        , _password(password)
    {
    }

    std::string username() const override { return _username; }
    std::string password() const override { return _password; }

protected:
    std::string _username;
    std::string _password;
};


using IUserMap = std::map<std::string, IUser*>;


/// @deprecated
/// This class manages a list of users.
class /* ICY_EXTERN */ UserManager : public LiveCollection<std::string, IUser>
{
public:
    using Manager = LiveCollection<std::string, IUser>;
    using Map = Manager::Map;

public:
    UserManager() = default;
    virtual ~UserManager() = default;

    virtual bool add(IUser* user)
    {
        return Manager::add(user->username(), user);
    };
};


/// @\}


} // namespace icy


/// @\}
