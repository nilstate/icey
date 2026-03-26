#pragma once


#include <string>
#include <unordered_set>
#include <vector>


namespace icy::smpl::detail {


bool noteDesiredJoin(std::unordered_set<std::string>& desiredRooms,
                     std::unordered_set<std::string>& pendingLeaves,
                     const std::string& room);

bool noteDesiredLeave(std::unordered_set<std::string>& desiredRooms,
                      const std::unordered_set<std::string>& currentRooms,
                      std::unordered_set<std::string>& pendingLeaves,
                      const std::string& room);

std::vector<std::string> collectJoinRequests(const std::unordered_set<std::string>& desiredRooms,
                                             const std::unordered_set<std::string>& currentRooms,
                                             const std::unordered_set<std::string>& pendingJoins);

bool applyJoinAck(const std::unordered_set<std::string>& desiredRooms,
                  std::unordered_set<std::string>& currentRooms,
                  std::unordered_set<std::string>& pendingJoins,
                  std::unordered_set<std::string>& pendingLeaves,
                  const std::string& room);

bool applyLeaveAck(const std::unordered_set<std::string>& desiredRooms,
                   std::unordered_set<std::string>& currentRooms,
                   std::unordered_set<std::string>& pendingJoins,
                   std::unordered_set<std::string>& pendingLeaves,
                   const std::string& room);

void applyWelcome(std::unordered_set<std::string>& currentRooms,
                  std::unordered_set<std::string>& pendingJoins,
                  std::unordered_set<std::string>& pendingLeaves,
                  const std::unordered_set<std::string>& welcomeRooms);

void resetTransientRoomState(std::unordered_set<std::string>& currentRooms,
                             std::unordered_set<std::string>& pendingJoins,
                             std::unordered_set<std::string>& pendingLeaves);


} // namespace icy::smpl::detail
