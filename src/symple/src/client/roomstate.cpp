///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//


#include "roomstate.h"


namespace icy::smpl::detail {


bool noteDesiredJoin(std::unordered_set<std::string>& desiredRooms,
                     std::unordered_set<std::string>& pendingLeaves,
                     const std::string& room)
{
    if (room.empty())
        return false;

    desiredRooms.insert(room);
    pendingLeaves.erase(room);
    return true;
}


bool noteDesiredLeave(std::unordered_set<std::string>& desiredRooms,
                      const std::unordered_set<std::string>& currentRooms,
                      std::unordered_set<std::string>& pendingLeaves,
                      const std::string& room)
{
    if (room.empty())
        return false;

    desiredRooms.erase(room);
    if (!currentRooms.count(room) || pendingLeaves.count(room))
        return false;

    pendingLeaves.insert(room);
    return true;
}


std::vector<std::string> collectJoinRequests(const std::unordered_set<std::string>& desiredRooms,
                                             const std::unordered_set<std::string>& currentRooms,
                                             const std::unordered_set<std::string>& pendingJoins)
{
    std::vector<std::string> rooms;
    rooms.reserve(desiredRooms.size());
    for (const auto& room : desiredRooms) {
        if (!currentRooms.count(room) && !pendingJoins.count(room))
            rooms.push_back(room);
    }
    return rooms;
}


bool applyJoinAck(const std::unordered_set<std::string>& desiredRooms,
                  std::unordered_set<std::string>& currentRooms,
                  std::unordered_set<std::string>& pendingJoins,
                  std::unordered_set<std::string>& pendingLeaves,
                  const std::string& room)
{
    pendingJoins.erase(room);
    currentRooms.insert(room);
    if (desiredRooms.count(room) || pendingLeaves.count(room))
        return false;

    pendingLeaves.insert(room);
    return true;
}


bool applyLeaveAck(const std::unordered_set<std::string>& desiredRooms,
                   std::unordered_set<std::string>& currentRooms,
                   std::unordered_set<std::string>& pendingJoins,
                   std::unordered_set<std::string>& pendingLeaves,
                   const std::string& room)
{
    pendingLeaves.erase(room);
    currentRooms.erase(room);
    return desiredRooms.count(room) && !pendingJoins.count(room);
}


void applyWelcome(std::unordered_set<std::string>& currentRooms,
                  std::unordered_set<std::string>& pendingJoins,
                  std::unordered_set<std::string>& pendingLeaves,
                  const std::unordered_set<std::string>& welcomeRooms)
{
    currentRooms = welcomeRooms;
    pendingJoins.clear();
    pendingLeaves.clear();
}


void resetTransientRoomState(std::unordered_set<std::string>& currentRooms,
                             std::unordered_set<std::string>& pendingJoins,
                             std::unordered_set<std::string>& pendingLeaves)
{
    currentRooms.clear();
    pendingJoins.clear();
    pendingLeaves.clear();
}


} // namespace icy::smpl::detail
