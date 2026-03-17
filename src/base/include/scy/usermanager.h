///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#ifndef SCY_UserManager_H
#define SCY_UserManager_H


#include "scy/collection.h"
#include <map>


namespace scy {


struct IUser
{
    virtual std::string username() const = 0;
    virtual std::string password() const = 0;
};


class /* SCY_EXTERN */ BasicUser : public IUser
{
public:
    BasicUser(const std::string& username, const std::string& password = "")
        : _username(username)
        , _password(password)
    {
    }

    std::string username() const { return _username; }
    std::string password() const { return _password; }

protected:
    std::string _username;
    std::string _password;
};


typedef std::map<std::string, IUser*> IUserMap;


/// @deprecated
/// This class manages a list of users.
class /* SCY_EXTERN */ UserManager : public LiveCollection<std::string, IUser>
{
public:
    typedef LiveCollection<std::string, IUser> Manager;
    typedef Manager::Map Map;

public:
    UserManager() = default;
    virtual ~UserManager() = default;

    virtual bool add(IUser* user)
    {
        return Manager::add(user->username(), user);
    };
};


/// @\}


} // namespace scy


#endif // SCY_UserManager_H


/// @\}
