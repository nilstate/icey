#include "icy/base.h"
#include "icy/logger.h"
#include "icy/stun/message.h"
#include "icy/test.h"
#include "icy/util.h"

#include <algorithm>
#include <cstring>
#include <map>
#include <stdexcept>
#include <typeinfo>


using namespace std;
using namespace icy;
using namespace icy::test;


int main(int argc, char** argv)
{
    // Logger::instance().add(std::make_unique<ConsoleChannel>("Test", Level::Trace));
    test::init();

    // =========================================================================
    // Message Integrity
    //
    describe("message integrity", []() {
        std::string username("someuser");
        std::string password("somepass");

        stun::Message request(stun::Message::Request, stun::Message::Allocate);

        request.add<stun::Username>().copyBytes(username.c_str(), username.size());
        request.add<stun::MessageIntegrity>().setKey(password);

        Buffer buf;
        request.write(buf);

        stun::Message response;
        expect(response.read(constBuffer(buf)) > 0);

        auto* integrity = response.get<stun::MessageIntegrity>();
        expect(integrity != nullptr);
        expect(integrity->verifyHmac(password));
    });

    // =========================================================================
    // Message Type Encoding: all class/method combos round-trip correctly
    //
    describe("message type encoding round-trip", []() {
        stun::Message::ClassType classes[] = {
            stun::Message::Request,
            stun::Message::Indication,
            stun::Message::SuccessResponse,
            stun::Message::ErrorResponse
        };
        stun::Message::MethodType methods[] = {
            stun::Message::Binding,
            stun::Message::Allocate,
            stun::Message::Refresh,
            stun::Message::SendIndication,
            stun::Message::DataIndication,
            stun::Message::CreatePermission,
            stun::Message::ChannelBind,
            stun::Message::Connect,
            stun::Message::ConnectionBind,
            stun::Message::ConnectionAttempt
        };

        for (auto cls : classes) {
            for (auto meth : methods) {
                stun::Message msg(cls, meth);
                expect(msg.classType() == cls);
                expect(msg.methodType() == meth);

                Buffer buf;
                msg.write(buf);

                stun::Message parsed;
                auto nread = parsed.read(constBuffer(buf));
                expect(nread > 0);
                expect(parsed.classType() == cls);
                expect(parsed.methodType() == meth);
                expect(parsed.transactionID() == msg.transactionID());
            }
        }
    });

    // =========================================================================
    // Request Types
    //
    describe("request types", []() {
        stun::Message request(stun::Message::Indication, stun::Message::SendIndication);
        expect(request.classType() != stun::Message::Request);
        expect(request.classType() == stun::Message::Indication);

        stun::Message request1(stun::Message::Request, stun::Message::Allocate);
        expect(request1.classType() == stun::Message::Request);
        expect(request1.methodType() == stun::Message::Allocate);
    });

    // =========================================================================
    // Xor Address - IPv4
    //
    describe("xor address ipv4", []() {
        expect(5555 == 0x15B3);
        expect((5555 ^ (stun::kMagicCookie >> 16)) == 0x34A1);

        net::Address addr("192.168.1.1", 5555);

        stun::Message request(stun::Message::Request, stun::Message::Allocate);

        request.add<stun::XorRelayedAddress>().setAddress(addr);

        Buffer buf;
        request.write(buf);

        stun::Message response;
        response.read(constBuffer(buf));

        auto* parsed = response.get<stun::XorRelayedAddress>();
        expect(parsed != nullptr);
        expect(parsed->address() == addr);
    });

    // =========================================================================
    // Xor Address - Various IPv4 addresses
    //
    describe("xor address various ipv4", []() {
        struct TestCase { const char* ip; uint16_t port; };
        TestCase cases[] = {
            {"0.0.0.0", 0},
            {"255.255.255.255", 65535},
            {"10.0.0.1", 3478},
            {"172.16.0.1", 1},
            {"127.0.0.1", 80},
            {"1.2.3.4", 443},
            {"192.168.100.200", 12345},
        };

        for (auto& tc : cases) {
            net::Address addr(tc.ip, tc.port);

            stun::Message msg(stun::Message::SuccessResponse, stun::Message::Binding);
            msg.add<stun::XorMappedAddress>().setAddress(addr);

            Buffer buf;
            msg.write(buf);

            stun::Message parsed;
            auto nread = parsed.read(constBuffer(buf));
            expect(nread > 0);

            auto attrIn = parsed.get<stun::XorMappedAddress>();
            expect(attrIn != nullptr);
            expect(attrIn->address() == addr);
        }
    });

    // =========================================================================
    // Binding Request Round-Trip
    //
    describe("binding request round-trip", []() {
        stun::Message request(stun::Message::Request, stun::Message::Binding);
        expect(request.classType() == stun::Message::Request);
        expect(request.methodType() == stun::Message::Binding);

        // Serialize
        Buffer buf;
        request.write(buf);
        expect(buf.size() >= 20); // STUN header is 20 bytes minimum

        // Deserialize
        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread == static_cast<ssize_t>(buf.size()));
        expect(parsed.classType() == stun::Message::Request);
        expect(parsed.methodType() == stun::Message::Binding);
        expect(parsed.transactionID() == request.transactionID());
    });


    // =========================================================================
    // Success Response
    //
    describe("success response", []() {
        stun::Message response(stun::Message::SuccessResponse, stun::Message::Binding);
        expect(response.classType() == stun::Message::SuccessResponse);
        expect(response.methodType() == stun::Message::Binding);

        // Add a mapped address
        net::Address addr("10.0.0.1", 3478);
        response.add<stun::XorMappedAddress>().setAddress(addr);

        // Round-trip
        Buffer buf;
        response.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));
        expect(parsed.classType() == stun::Message::SuccessResponse);

        auto parsedAddr = parsed.get<stun::XorMappedAddress>();
        expect(parsedAddr != nullptr);
        expect(parsedAddr->address() == addr);
    });


    // =========================================================================
    // Multiple Attributes
    //
    describe("multiple attributes", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        std::string user = "testuser";
        msg.add<stun::Username>().copyBytes(user.c_str(), user.size());

        net::Address addr("192.168.1.1", 5555);
        msg.add<stun::XorRelayedAddress>().setAddress(addr);

        // Serialize and parse back
        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto parsedUser = parsed.get<stun::Username>();
        expect(parsedUser != nullptr);

        auto parsedAddr = parsed.get<stun::XorRelayedAddress>();
        expect(parsedAddr != nullptr);
        expect(parsedAddr->address() == addr);
    });


    // =========================================================================
    // Copy Constructor
    //
    describe("message copy", []() {
        stun::Message original(stun::Message::Request, stun::Message::Binding);

        stun::Message copy(original);
        expect(copy.classType() == original.classType());
        expect(copy.methodType() == original.methodType());
        expect(copy.transactionID() == original.transactionID());
    });


    // =========================================================================
    // Move Constructor
    //
    describe("message move", []() {
        stun::Message original(stun::Message::ErrorResponse, stun::Message::Allocate);
        auto tid = original.transactionID();

        // Add an attribute to test that it moves
        original.add<stun::Lifetime>().setValue(300);

        stun::Message moved(std::move(original));
        expect(moved.classType() == stun::Message::ErrorResponse);
        expect(moved.methodType() == stun::Message::Allocate);
        expect(moved.transactionID() == tid);
        expect(moved.get<stun::Lifetime>() != nullptr);
        expect(moved.get<stun::Lifetime>()->value() == 300);

        // Moved-from object should be in a valid default state
        expect(original.classType() == stun::Message::Request);
        expect(original.methodType() == stun::Message::Undefined);
    });


    // =========================================================================
    // Method and Class Strings
    //
    describe("method and class strings", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        expect(msg.methodString() == "BINDING");
        expect(msg.classString() == "Request");

        stun::Message msg2(stun::Message::ErrorResponse, stun::Message::Allocate);
        expect(msg2.classString() == "ErrorResponse");
        expect(msg2.methodString() == "ALLOCATE");
        expect(!msg2.errorString(stun::Message::NotAuthorized).empty());

        // Check all new error codes have strings
        expect(msg2.errorString(stun::Message::StaleNonce) == "Stale Nonce");
        expect(msg2.errorString(stun::Message::Forbidden) == "Forbidden");
        expect(msg2.errorString(stun::Message::AllocationMismatch) == "Allocation Mismatch");
    });

    // =========================================================================
    // Attribute Factory
    //
    describe("attribute factory", []() {
        // Create known attribute types via factory
        auto username = stun::Attribute::create(stun::Attribute::Username, 0);
        expect(username != nullptr);
        expect(username->type() == stun::Attribute::Username);

        auto lifetime = stun::Attribute::create(stun::Attribute::Lifetime, 4);
        expect(lifetime != nullptr);
        expect(lifetime->type() == stun::Attribute::Lifetime);

        auto realm = stun::Attribute::create(stun::Attribute::Realm, 0);
        expect(realm != nullptr);

        auto nonce = stun::Attribute::create(stun::Attribute::Nonce, 0);
        expect(nonce != nullptr);

        auto software = stun::Attribute::create(stun::Attribute::Software, 0);
        expect(software != nullptr);

        auto responseAddress = stun::Attribute::create(
            stun::Attribute::ResponseAddress, stun::AddressAttribute::IPv4Size);
        expect(responseAddress != nullptr);

        auto sourceAddress = stun::Attribute::create(
            stun::Attribute::SourceAddress, stun::AddressAttribute::IPv6Size);
        expect(sourceAddress != nullptr);
        expect(sourceAddress->size() == stun::AddressAttribute::IPv6Size);

        auto destinationAddress = stun::Attribute::create(
            stun::Attribute::DestinationAddress, stun::AddressAttribute::IPv4Size);
        expect(destinationAddress != nullptr);

        auto changeRequest = stun::Attribute::create(
            stun::Attribute::ChangeRequest, stun::ChangeRequest::Size);
        expect(changeRequest != nullptr);

        auto options = stun::Attribute::create(
            stun::Attribute::Options, stun::Options::Size);
        expect(options != nullptr);

        // Unknown type returns nullptr
        auto unknown = stun::Attribute::create(0xFFFF, 4);
        expect(unknown == nullptr);

        // Invalid size for known type returns nullptr
        auto badLifetime = stun::Attribute::create(stun::Attribute::Lifetime, 3);
        expect(badLifetime == nullptr);

        auto badUnknownAttrs = stun::Attribute::create(
            stun::Attribute::UnknownAttributes, 3);
        expect(badUnknownAttrs == nullptr);
    });


    // =========================================================================
    // UInt32 Attribute (Lifetime)
    //
    describe("uint32 attribute lifetime", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        msg.add<stun::Lifetime>().setValue(600);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto* lt = parsed.get<stun::Lifetime>();
        expect(lt != nullptr);
        expect(lt->value() == 600);
    });


    // =========================================================================
    // UInt64 Attribute (ICE Controlling)
    //
    describe("uint64 attribute ice controlling", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);

        msg.add<stun::ICEControlling>().setValue(0xDEADBEEF12345678ULL);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedAttr = parsed.get<stun::ICEControlling>();
        expect(parsedAttr != nullptr);
        expect(parsedAttr->value() == 0xDEADBEEF12345678ULL);
    });


    // =========================================================================
    // UInt8 Attribute (EventPort) - tests padding
    //
    describe("uint8 attribute with padding", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        msg.add<stun::EventPort>().setValue(0x80);
        msg.add<stun::Lifetime>().setValue(42);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedEvent = parsed.get<stun::EventPort>();
        expect(parsedEvent != nullptr);
        expect(parsedEvent->value() == 0x80);

        auto parsedLifetime = parsed.get<stun::Lifetime>();
        expect(parsedLifetime != nullptr);
        expect(parsedLifetime->value() == 42);
    });


    // =========================================================================
    // Flag Attribute (DontFragment, ICEUseCandidate)
    //
    describe("flag attributes", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        msg.add<stun::DontFragment>();
        msg.add<stun::ICEUseCandidate>();
        msg.add<stun::Lifetime>().setValue(99);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        expect(parsed.get<stun::DontFragment>() != nullptr);
        expect(parsed.get<stun::ICEUseCandidate>() != nullptr);
        expect(parsed.get<stun::Lifetime>() != nullptr);
        expect(parsed.get<stun::Lifetime>()->value() == 99);
    });


    // =========================================================================
    // String Attribute (Software)
    //
    describe("string attribute software", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);

        std::string sw = "Icey STUN";
        msg.add<stun::Software>().copyBytes(sw.c_str(), sw.size());

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto* parsedSw = parsed.get<stun::Software>();
        expect(parsedSw != nullptr);
        expect(parsedSw->asString() == sw);
    });


    // =========================================================================
    // String Attribute - odd length (tests padding)
    //
    describe("string attribute odd length padding", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);

        std::string sw = "1234567";
        msg.add<stun::Software>().copyBytes(sw.c_str(), sw.size());

        std::string user = "hello";
        msg.add<stun::Username>().copyBytes(user.c_str(), user.size());

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedSw = parsed.get<stun::Software>();
        expect(parsedSw != nullptr);
        expect(parsedSw->asString() == sw);

        auto parsedUser = parsed.get<stun::Username>();
        expect(parsedUser != nullptr);
        expect(parsedUser->asString() == user);
    });


    // =========================================================================
    // Error Code Attribute
    //
    describe("error code attribute", []() {
        stun::Message msg(stun::Message::ErrorResponse, stun::Message::Allocate);

        auto& err = msg.add<stun::ErrorCode>();
        err.setErrorCode(401);
        err.setReason("Unauthorized");

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto parsedErr = parsed.get<stun::ErrorCode>();
        expect(parsedErr != nullptr);
        expect(parsedErr->errorCode() == 401);
        expect(parsedErr->errorClass() == 4);
        expect(parsedErr->errorNumber() == 1);
        expect(parsedErr->reason() == "Unauthorized");
    });


    // =========================================================================
    // Error Code - various codes
    //
    describe("error code various", []() {
        int codes[] = {300, 400, 401, 403, 420, 437, 438, 441, 442, 486, 487, 500, 508, 600};
        for (int code : codes) {
            stun::Message msg(stun::Message::ErrorResponse, stun::Message::Binding);

            auto& err = msg.add<stun::ErrorCode>();
            err.setErrorCode(code);
            err.setReason("test");

            Buffer buf;
            msg.write(buf);

            stun::Message parsed;
            auto nread = parsed.read(constBuffer(buf));
            expect(nread > 0);

            auto parsedErr = parsed.get<stun::ErrorCode>();
            expect(parsedErr != nullptr);
            expect(parsedErr->errorCode() == code);
            expect(parsedErr->errorClass() == code / 100);
            expect(parsedErr->errorNumber() == code % 100);
        }
    });


    // =========================================================================
    // Multiple Address Attributes
    //
    describe("multiple address types", []() {
        stun::Message msg(stun::Message::SuccessResponse, stun::Message::Allocate);

        net::Address relay("10.0.0.1", 5000);
        msg.add<stun::XorRelayedAddress>().setAddress(relay);

        net::Address mapped("192.168.1.100", 6000);
        msg.add<stun::XorMappedAddress>().setAddress(mapped);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto parsedRelay = parsed.get<stun::XorRelayedAddress>();
        expect(parsedRelay != nullptr);
        expect(parsedRelay->address() == relay);

        auto parsedMapped = parsed.get<stun::XorMappedAddress>();
        expect(parsedMapped != nullptr);
        expect(parsedMapped->address() == mapped);
    });


    // =========================================================================
    // Attribute Type Strings
    //
    describe("attribute type string", []() {
        auto username = stun::Attribute::create(stun::Attribute::Username, 0);
        expect(!username->typeString().empty());
        expect(username->typeString() == "USERNAME");
    });


    // =========================================================================
    // Malformed Packet: bad magic cookie
    //
    describe("reject bad magic cookie", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        Buffer buf;
        msg.write(buf);

        // Corrupt the magic cookie (bytes 4-7 in the header)
        buf[4] = 0xFF;
        buf[5] = 0xFF;

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread == 0); // should reject
    });


    // =========================================================================
    // Malformed Packet: unaligned message length
    //
    describe("reject unaligned message length", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        Buffer buf;
        msg.write(buf);

        // Set message length to 3 (not a multiple of 4) at bytes 2-3
        buf[2] = 0x00;
        buf[3] = 0x03;

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread == 0);
    });


    // =========================================================================
    // Malformed Packet: truncated buffer
    //
    describe("reject truncated buffer", []() {
        // Buffer too small for even a STUN header (20 bytes)
        char small[10] = {};
        stun::Message parsed;
        auto nread = parsed.read(constBuffer(small, 10));
        expect(nread == 0);
    });


    // =========================================================================
    // Malformed Packet: MSB set (looks like RTP)
    //
    describe("reject rtp-like packet", []() {
        // First two bits set = not STUN
        char data[20] = {};
        data[0] = static_cast<char>(0xC0); // both top bits set
        stun::Message parsed;
        auto nread = parsed.read(constBuffer(data, 20));
        expect(nread == 0);
    });


    // =========================================================================
    // Malformed Packet: message length exceeds buffer
    //
    describe("reject oversized message length", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        Buffer buf;
        msg.write(buf);

        // Set message length to something huge
        buf[2] = 0x0F;
        buf[3] = 0xFC; // 4092 bytes, way larger than our buffer

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread == 0);
    });


    // =========================================================================
    // Allocate request with full attribute set
    //
    describe("allocate request full attributes", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        msg.add<stun::RequestedTransport>().setValue(17 << 24); // UDP
        msg.add<stun::Lifetime>().setValue(600);

        std::string user = "testuser";
        msg.add<stun::Username>().copyBytes(user.c_str(), user.size());

        std::string r = "example.com";
        msg.add<stun::Realm>().copyBytes(r.c_str(), r.size());

        std::string n = "somenonce123";
        msg.add<stun::Nonce>().copyBytes(n.c_str(), n.size());

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);
        expect(parsed.methodType() == stun::Message::Allocate);

        expect(parsed.get<stun::RequestedTransport>() != nullptr);
        expect(parsed.get<stun::RequestedTransport>()->value() == (17u << 24));

        expect(parsed.get<stun::Lifetime>() != nullptr);
        expect(parsed.get<stun::Lifetime>()->value() == 600);

        expect(parsed.get<stun::Username>() != nullptr);
        expect(parsed.get<stun::Username>()->asString() == "testuser");

        expect(parsed.get<stun::Realm>() != nullptr);
        expect(parsed.get<stun::Realm>()->asString() == "example.com");

        expect(parsed.get<stun::Nonce>() != nullptr);
        expect(parsed.get<stun::Nonce>()->asString() == "somenonce123");
    });


    // =========================================================================
    // Multiple XorPeerAddress attributes (CreatePermission style)
    //
    describe("multiple xor peer addresses", []() {
        stun::Message msg(stun::Message::Request, stun::Message::CreatePermission);

        net::Address peer1("10.0.0.1", 0);
        net::Address peer2("10.0.0.2", 0);
        net::Address peer3("10.0.0.3", 0);

        msg.add<stun::XorPeerAddress>().setAddress(peer1);
        msg.add<stun::XorPeerAddress>().setAddress(peer2);
        msg.add<stun::XorPeerAddress>().setAddress(peer3);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto p0 = parsed.get<stun::XorPeerAddress>(0);
        auto p1 = parsed.get<stun::XorPeerAddress>(1);
        auto p2 = parsed.get<stun::XorPeerAddress>(2);
        auto p3 = parsed.get<stun::XorPeerAddress>(3);

        expect(p0 != nullptr);
        expect(p1 != nullptr);
        expect(p2 != nullptr);
        expect(p3 == nullptr); // only 3 added

        expect(p0->address().host() == "10.0.0.1");
        expect(p1->address().host() == "10.0.0.2");
        expect(p2->address().host() == "10.0.0.3");
    });


    // =========================================================================
    // Attribute clone
    //
    describe("attribute clone", []() {
        auto original = std::make_unique<stun::Lifetime>();
        original->setValue(42);

        auto cloned = original->clone();
        expect(cloned != nullptr);
        expect(cloned->type() == stun::Attribute::Lifetime);

        // clone() returns UInt32Attribute (base), not Lifetime (derived macro type)
        auto* clonedU32 = static_cast<stun::UInt32Attribute*>(cloned.get());
        expect(clonedU32 != nullptr);
        expect(clonedU32->value() == 42);
    });


    // =========================================================================
    // ConnectionID attribute round-trip
    //
    describe("connection id attribute", []() {
        stun::Message msg(stun::Message::Request, stun::Message::ConnectionBind);

        msg.add<stun::ConnectionID>().setValue(0x12345678);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedConn = parsed.get<stun::ConnectionID>();
        expect(parsedConn != nullptr);
        expect(parsedConn->value() == 0x12345678);
    });


    // =========================================================================
    // ChannelNumber attribute
    //
    describe("channel number attribute", []() {
        stun::Message msg(stun::Message::Request, stun::Message::ChannelBind);

        msg.add<stun::ChannelNumber>().setValue(0x4000 << 16);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedCh = parsed.get<stun::ChannelNumber>();
        expect(parsedCh != nullptr);
        expect(parsedCh->value() == (0x4000u << 16));
    });


    // =========================================================================
    // ReservationToken attribute (8 bytes)
    //
    describe("reservation token attribute", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        msg.add<stun::ReservationToken>().copyBytes("ABCDEFGH", 8);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto parsedToken = parsed.get<stun::ReservationToken>();
        expect(parsedToken != nullptr);
        expect(parsedToken->asString() == "ABCDEFGH");
    });


    // =========================================================================
    // UNKNOWN-ATTRIBUTES parse: wire bytes should populate the list
    //
    describe("unknown attributes parse", []() {
        stun::Message msg(stun::Message::ErrorResponse, stun::Message::Binding);

        // Add two unknown attribute type codes
        auto& ua = msg.add<stun::UnknownAttributes>();
        ua.addType(0x0017);
        ua.addType(0x8028);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto* parsedUA = parsed.get<stun::UnknownAttributes>();
        expect(parsedUA != nullptr);
        expect(parsedUA->size() == 2);
        expect(parsedUA->getType(0) == 0x0017);
        expect(parsedUA->getType(1) == 0x8028);
    });


    // =========================================================================
    // Plain (non-XOR) address attributes: MAPPED-ADDRESS round-trip
    //
    describe("plain mapped address ipv4", []() {
        net::Address addr("203.0.113.42", 3478);

        stun::Message msg(stun::Message::SuccessResponse, stun::Message::Binding);
        msg.add<stun::MappedAddress>().setAddress(addr);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto* ma = parsed.get<stun::MappedAddress>();
        expect(ma != nullptr);
        expect(ma->address() == addr);
    });


    // =========================================================================
    // IPv6 address attributes: size must follow the address family on write/read
    //
    describe("mapped address ipv6 round-trip", []() {
        net::Address addr("2001:db8::42", 3478);

        stun::Message msg(stun::Message::SuccessResponse, stun::Message::Binding);
        auto& mapped = msg.add<stun::MappedAddress>();
        mapped.setAddress(addr);
        expect(mapped.size() == stun::AddressAttribute::IPv6Size);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto* parsedAddr = parsed.get<stun::MappedAddress>();
        expect(parsedAddr != nullptr);
        expect(parsedAddr->size() == stun::AddressAttribute::IPv6Size);
        expect(parsedAddr->address() == addr);
    });


    // =========================================================================
    // Legacy declared attributes must be parseable through Attribute::create()
    //
    describe("legacy declared attrs round-trip", []() {
        net::Address source("2001:db8::2", 3478);
        net::Address destination("203.0.113.20", 5000);

        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        msg.add<stun::SourceAddress>().setAddress(source);
        msg.add<stun::DestinationAddress>().setAddress(destination);
        msg.add<stun::ChangeRequest>().setValue(0x00000006);
        msg.add<stun::Options>().setValue(0x12345678);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        auto nread = parsed.read(constBuffer(buf));
        expect(nread > 0);

        auto* parsedSource = parsed.get<stun::SourceAddress>();
        expect(parsedSource != nullptr);
        expect(parsedSource->address() == source);

        auto* parsedDestination = parsed.get<stun::DestinationAddress>();
        expect(parsedDestination != nullptr);
        expect(parsedDestination->address() == destination);

        auto* parsedChange = parsed.get<stun::ChangeRequest>();
        expect(parsedChange != nullptr);
        expect(parsedChange->value() == 0x00000006u);

        auto* parsedOptions = parsed.get<stun::Options>();
        expect(parsedOptions != nullptr);
        expect(parsedOptions->value() == 0x12345678u);
    });


    // =========================================================================
    // Message copies must preserve concrete attribute types
    //
    describe("message copy preserves concrete attribute types", []() {
        net::Address addr("2001:db8::9", 4444);

        stun::Message original(stun::Message::Request, stun::Message::Binding);
        original.add<stun::Lifetime>().setValue(600);
        original.add<stun::Username>().copyBytes("alice", 5);
        original.add<stun::XorMappedAddress>().setAddress(addr);

        stun::Message copy(original);

        auto* lifetime = copy.get(stun::Attribute::Lifetime);
        expect(lifetime != nullptr);
        expect(typeid(*lifetime) == typeid(stun::Lifetime));
        expect(copy.get<stun::Lifetime>()->value() == 600);

        auto* username = copy.get(stun::Attribute::Username);
        expect(username != nullptr);
        expect(typeid(*username) == typeid(stun::Username));
        expect(copy.get<stun::Username>()->asString() == "alice");

        auto* mapped = copy.get(stun::Attribute::XorMappedAddress);
        expect(mapped != nullptr);
        expect(typeid(*mapped) == typeid(stun::XorMappedAddress));
        expect(copy.get<stun::XorMappedAddress>()->address() == addr);
    });


    // =========================================================================
    // XOR vs plain address: verify XOR-MAPPED != MAPPED on the wire
    //
    describe("xor vs plain address wire encoding differs", []() {
        net::Address addr("198.51.100.1", 9999);

        stun::Message xorMsg(stun::Message::SuccessResponse, stun::Message::Binding);
        xorMsg.add<stun::XorMappedAddress>().setAddress(addr);
        Buffer xorBuf;
        xorMsg.write(xorBuf);

        stun::Message plainMsg(stun::Message::SuccessResponse, stun::Message::Binding);
        plainMsg.add<stun::MappedAddress>().setAddress(addr);
        Buffer plainBuf;
        plainMsg.write(plainBuf);

        // Same address but wire bytes must differ (XOR obfuscation)
        expect(xorBuf.size() == plainBuf.size());
        bool differ = false;
        for (size_t i = 0; i < xorBuf.size(); i++) {
            if (xorBuf[i] != plainBuf[i]) { differ = true; break; }
        }
        expect(differ);

        // Both must parse back to the same address
        stun::Message parsedXor;
        parsedXor.read(constBuffer(xorBuf));
        stun::Message parsedPlain;
        parsedPlain.read(constBuffer(plainBuf));

        expect(parsedXor.get<stun::XorMappedAddress>()->address() == addr);
        expect(parsedPlain.get<stun::MappedAddress>()->address() == addr);
    });


    test::runAll();
    return test::finalize();
}
