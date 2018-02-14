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
    // Logger::instance().add(new ConsoleChannel("Test", Level::Trace));
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
        // LDebug("Source Address: ", addr)

        stun::Message request(stun::Message::Request, stun::Message::Allocate);
        // stun::Message request;
        // request.setType(stun::Message::Allocate);

        auto addrAttr = new stun::XorRelayedAddress;
        addrAttr->setAddress(addr);
        request.add(addrAttr);
        // LDebug("Request Address: ", addrAttr->address())

        Buffer buf;
        request.write(buf);

        stun::Message response;
        response.read(constBuffer(buf));

        addrAttr = response.get<stun::XorRelayedAddress>();

        // LDebug("Response Address: ", addrAttr->address())
        expect(addrAttr->address() == addr);
    });

    test::runAll();
    return test::finalize();
}
