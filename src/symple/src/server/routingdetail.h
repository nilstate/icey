#pragma once


#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>


namespace icy::smpl::detail {


using RoomMemberSet = std::unordered_set<std::string>;
using RoomMemberMap = std::unordered_map<std::string, RoomMemberSet>;


inline bool sharesAnyRoom(const std::unordered_set<std::string>& a,
                          const std::unordered_set<std::string>& b)
{
    for (const auto& room : a) {
        if (b.count(room))
            return true;
    }
    return false;
}


inline std::unordered_set<std::string> collectRecipients(const RoomMemberMap& indexedRooms,
                                                         const std::unordered_set<std::string>& rooms,
                                                         std::string_view excludeId = {})
{
    size_t expectedRecipients = 0;
    for (const auto& room : rooms) {
        auto it = indexedRooms.find(room);
        if (it != indexedRooms.end())
            expectedRecipients += it->second.size();
    }

    std::unordered_set<std::string> recipients;
    recipients.reserve(expectedRecipients);
    for (const auto& room : rooms) {
        auto it = indexedRooms.find(room);
        if (it == indexedRooms.end())
            continue;
        for (const auto& peerId : it->second) {
            if (peerId != excludeId)
                recipients.insert(peerId);
        }
    }
    return recipients;
}


} // namespace icy::smpl::detail
