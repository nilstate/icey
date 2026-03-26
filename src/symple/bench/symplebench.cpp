#include "icy/symple/address.h"
#include "icy/symple/message.h"
#include "icy/symple/peer.h"
#include "icy/symple/roster.h"

#include "../src/client/roomstate.h"
#include "../src/client/rosterstate.h"
#include "../src/protocol.h"
#include "../src/server/routingdetail.h"
#include "benchutil.h"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using namespace icy;
using namespace icy::smpl;

namespace {

detail::RoomMemberMap makeRoomMap(size_t roomCount,
                                  size_t peersPerRoom,
                                  size_t overlap)
{
    detail::RoomMemberMap map;
    for (size_t roomIndex = 0; roomIndex < roomCount; ++roomIndex) {
        auto& room = map["room-" + std::to_string(roomIndex)];
        room.reserve(peersPerRoom);
        for (size_t memberIndex = 0; memberIndex < peersPerRoom; ++memberIndex) {
            const size_t peerIndex = memberIndex + roomIndex * overlap;
            room.insert("peer-" + std::to_string(peerIndex));
        }
    }
    return map;
}


void benchRoomFanout(bench::Reporter& reporter)
{
    const auto indexedRooms = makeRoomMap(8, 256, 64);
    const std::unordered_set<std::string> rooms = {"room-1", "room-2", "room-3", "room-4"};

    json::Value message = {
        {"type", "message"},
        {"from", "sender|sender"},
        {"data", {
            {"kind", "bench"},
            {"value", 42},
        }},
    };

    constexpr uint64_t iterations = 20000;
    uint64_t totalRecipients = 0;
    uint64_t totalBytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        totalRecipients = 0;
        totalBytes = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            auto recipients = detail::collectRecipients(indexedRooms, rooms, "peer-0");
            auto payload = message.dump();
            totalRecipients += recipients.size();
            totalBytes += recipients.size() * payload.size();
        }
    });

    reporter.add({
        .name = "symple room fanout",
        .measurement = measurement,
        .metrics = {bench::metric("rooms", static_cast<uint64_t>(rooms.size())),
                    bench::metric("avg_recipients", totalRecipients / iterations),
                    bench::metric("avg_bytes", totalBytes / iterations)},
    });
}


void dispatchClientPayload(const std::string& payload,
                           Roster& roster,
                           std::unordered_set<std::string>& desiredRooms,
                           std::unordered_set<std::string>& currentRooms,
                           std::unordered_set<std::string>& pendingJoins,
                           std::unordered_set<std::string>& pendingLeaves)
{
    auto msg = json::Value::parse(payload);
    const auto type = msg.value("type", "");

    if (type == "welcome") {
        std::string error;
        auto welcome = proto::parseWelcome(msg, &error);
        if (!welcome)
            throw std::runtime_error(error.empty() ? "Invalid welcome" : error);

        auto update = detail::applyPresenceData(roster, static_cast<const json::Value&>(welcome->peer));
        if (update.kind == detail::PresenceUpdate::Kind::Invalid)
            throw std::runtime_error(update.error);

        detail::applyWelcome(currentRooms, pendingJoins, pendingLeaves, welcome->rooms);
        return;
    }

    if (auto room = proto::parseRoomMessage(msg, "join:ok")) {
        detail::applyJoinAck(
            desiredRooms, currentRooms, pendingJoins, pendingLeaves, *room);
        return;
    }

    if (auto room = proto::parseRoomMessage(msg, "leave:ok")) {
        detail::applyLeaveAck(
            desiredRooms, currentRooms, pendingJoins, pendingLeaves, *room);
        return;
    }

    if (type == "presence" && msg.contains("data")) {
        auto update = detail::applyPresenceData(roster, msg["data"]);
        if (update.kind == detail::PresenceUpdate::Kind::Invalid)
            throw std::runtime_error(update.error);
        return;
    }

    if (type == "message") {
        Message message(msg);
        if (!message.valid())
            throw std::runtime_error("Invalid message");
    }
}


void benchClientDispatch(bench::Reporter& reporter)
{
    Peer self;
    self["user"] = "alice";
    self["name"] = "Alice";
    self["type"] = "user";
    self["id"] = "alice-1";
    self["online"] = true;

    Peer remote;
    remote["user"] = "bob";
    remote["name"] = "Bob";
    remote["type"] = "user";

    const std::array<std::string, 5> payloads = {
        proto::makeWelcome(self, {"alpha", "beta"}).dump(),
        proto::makeRoomMessage("join:ok", "alpha").dump(),
        proto::makePresence(remote, "bob-1", true).dump(),
        proto::makePresence(remote, "bob-1", false).dump(),
        json::Value({
            {"type", "message"},
            {"from", "bob|bob-1"},
            {"to", "alice|alice-1"},
            {"data", {{"text", "hello"}}},
        }).dump(),
    };

    constexpr uint64_t batches = 10000;
    constexpr uint64_t iterations = batches * payloads.size();
    uint64_t bytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        Roster roster;
        std::unordered_set<std::string> desiredRooms = {"alpha"};
        std::unordered_set<std::string> currentRooms;
        std::unordered_set<std::string> pendingJoins = {"alpha"};
        std::unordered_set<std::string> pendingLeaves;
        bytes = 0;

        for (uint64_t batch = 0; batch < batches; ++batch) {
            for (const auto& payload : payloads) {
                bytes += payload.size();
                dispatchClientPayload(
                    payload,
                    roster,
                    desiredRooms,
                    currentRooms,
                    pendingJoins,
                    pendingLeaves);
            }
        }
    });

    reporter.add({
        .name = "symple client packet parse dispatch",
        .measurement = measurement,
        .metrics = {bench::metric("messages", iterations),
                    bench::metric("avg_bytes", bytes / iterations)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);
    benchRoomFanout(reporter);
    benchClientDispatch(reporter);
    return reporter.finish();
}
