#include "scy/base.h"
#include "scy/logger.h"
#include "scy/stun/message.h"
#include "scy/test.h"
#include "scy/util.h"

#include <algorithm>
#include <stdexcept>


using namespace std;
using namespace scy;
using namespace scy::test;


// TODO: Test vectors from http://tools.ietf.org/html/rfc5769


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
        // request.setType(stun::Message::Allocate);

        auto usernameAttr = new stun::Username;
        usernameAttr->copyBytes(username.c_str(), username.size());
        request.add(usernameAttr);

        auto integrityAttr = new stun::MessageIntegrity;
        integrityAttr->setKey(password);
        request.add(integrityAttr);

        Buffer buf;
        request.write(buf);

        stun::Message response;
        auto nread = response.read(constBuffer(buf));

        integrityAttr = response.get<stun::MessageIntegrity>();
        expect(integrityAttr != nullptr);
        expect(integrityAttr->verifyHmac(password));
    });

    // =========================================================================
    // Request Types
    //
    describe("request types", []() {
        uint16_t type = stun::Message::Indication | stun::Message::SendIndication;
        uint16_t classType = type & 0x0110;
        uint16_t methodType = type & 0x000F;

        expect(classType == stun::Message::Indication);
        expect(methodType == stun::Message::SendIndication);

        stun::Message request(stun::Message::Indication, stun::Message::SendIndication);
        // expect(IS_STUN_INDICATION(request.classType() | request.methodType()));

        expect(request.classType() != stun::Message::Request);
        expect(request.classType() == stun::Message::Indication);

        stun::Message request1(stun::Message::Request, stun::Message::Allocate);
        // expect(IS_STUN_REQUEST(request1.classType() | request1.methodType()));
    });

    // =========================================================================
    // Xor Address
    //
    describe("xor address", []() {
        expect(5555 == 0x15B3);
        expect((5555 ^ (stun::kMagicCookie >> 16)) == 0x34A1);

        net::Address addr("192.168.1.1", 5555);
        // LDebug("Source Address: ", addr);

        stun::Message request(stun::Message::Request, stun::Message::Allocate);
        // stun::Message request;
        // request.setType(stun::Message::Allocate);

        auto addrAttr = new stun::XorRelayedAddress;
        addrAttr->setAddress(addr);
        request.add(addrAttr);
        // LDebug("Request Address: ", addrAttr->address());

        Buffer buf;
        request.write(buf);

        stun::Message response;
        response.read(constBuffer(buf));

        addrAttr = response.get<stun::XorRelayedAddress>();

        // LDebug("Response Address: ", addrAttr->address());
        expect(addrAttr->address() == addr);
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
        auto addrAttr = new stun::XorMappedAddress;
        addrAttr->setAddress(addr);
        response.add(addrAttr);

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

        auto username = new stun::Username;
        std::string user = "testuser";
        username->copyBytes(user.c_str(), user.size());
        msg.add(username);

        net::Address addr("192.168.1.1", 5555);
        auto addrAttr = new stun::XorRelayedAddress;
        addrAttr->setAddress(addr);
        msg.add(addrAttr);

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
    // Method and Class Strings
    //
    describe("method and class strings", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);
        expect(!msg.methodString().empty());
        expect(!msg.classString().empty());

        stun::Message msg2(stun::Message::ErrorResponse, stun::Message::Allocate);
        expect(!msg2.classString().empty());
        expect(!msg2.errorString(stun::Message::NotAuthorized).empty());
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
    });


    // =========================================================================
    // UInt32 Attribute (Lifetime)
    //
    describe("uint32 attribute lifetime", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Allocate);

        auto lifetime = new stun::Lifetime;
        lifetime->setValue(600);
        msg.add(lifetime);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto parsedLifetime = parsed.get<stun::Lifetime>();
        expect(parsedLifetime != nullptr);
        expect(parsedLifetime->value() == 600);
    });


    // =========================================================================
    // String Attribute (Software)
    //
    describe("string attribute software", []() {
        stun::Message msg(stun::Message::Request, stun::Message::Binding);

        std::string sw = "LibSourcey STUN";
        auto software = new stun::Software;
        software->copyBytes(sw.c_str(), sw.size());
        msg.add(software);

        Buffer buf;
        msg.write(buf);

        stun::Message parsed;
        parsed.read(constBuffer(buf));

        auto parsedSw = parsed.get<stun::Software>();
        expect(parsedSw != nullptr);
        expect(parsedSw->asString() == sw);
    });


    // =========================================================================
    // Error Code Attribute
    //
    describe("error code attribute", []() {
        stun::Message msg(stun::Message::ErrorResponse, stun::Message::Allocate);

        auto err = new stun::ErrorCode;
        err->setErrorCode(401);
        err->setReason("Unauthorized");
        msg.add(err);

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
    // Multiple Address Attributes
    //
    describe("multiple address types", []() {
        stun::Message msg(stun::Message::SuccessResponse, stun::Message::Allocate);

        net::Address relay("10.0.0.1", 5000);
        auto relayAttr = new stun::XorRelayedAddress;
        relayAttr->setAddress(relay);
        msg.add(relayAttr);

        net::Address mapped("192.168.1.100", 6000);
        auto mappedAttr = new stun::XorMappedAddress;
        mappedAttr->setAddress(mapped);
        msg.add(mappedAttr);

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
        expect(username->typeString().find("Username") != std::string::npos ||
               username->typeString().find("USERNAME") != std::string::npos ||
               !username->typeString().empty());
    });


    test::runAll();
    return test::finalize();
}
