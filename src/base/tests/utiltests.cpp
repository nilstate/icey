#include "icy/base.h"
#include "icy/iregistry.h"
#include "icy/platform.h"
#include "icy/ratelimiter.h"
#include "icy/test.h"
#include "icy/timeout.h"

#include <thread>


using namespace std;
using namespace icy;
using namespace icy::test;


// =============================================================================
// Test types for IRegistry
//
struct Animal
{
    virtual ~Animal() = default;
    virtual std::string speak() const = 0;
};

struct Dog : public Animal
{
    std::string speak() const override { return "woof"; }
};

struct Cat : public Animal
{
    std::string speak() const override { return "meow"; }
};


int main(int argc, char** argv)
{
    test::init();

    // =========================================================================
    // Timeout - basic lifecycle
    //
    describe("timeout basic", []() {
        Timeout t(100);
        expect(!t.running());
        expect(!t.expired());

        t.start();
        expect(t.running());
        expect(!t.expired());
        expect(t.remaining() > 0);
        expect(t.remaining() <= 100);
    });


    // =========================================================================
    // Timeout - expiry
    //
    describe("timeout expiry", []() {
        Timeout t(50, true); // 50ms, autoStart
        expect(t.running());

        // Wait for expiry
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        expect(t.expired());
        expect(t.remaining() == 0);
    });


    // =========================================================================
    // Timeout - reset
    //
    describe("timeout reset", []() {
        Timeout t(50, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        expect(t.expired());

        t.reset();
        expect(t.running());
        expect(!t.expired());
        expect(t.remaining() > 0);
    });


    // =========================================================================
    // Timeout - stop
    //
    describe("timeout stop", []() {
        Timeout t(1000, true);
        expect(t.running());

        t.stop();
        expect(!t.running());
    });


    // =========================================================================
    // Timeout - copy
    //
    describe("timeout copy", []() {
        Timeout t1(200);
        t1.start();

        Timeout t2(t1);
        expect(t2.running());
        expect(t2.delay() == 200);
    });


    // =========================================================================
    // Timeout - assignment
    //
    describe("timeout assignment", []() {
        Timeout t1(300, true);
        Timeout t2;

        t2 = t1;
        expect(t2.delay() == 300);
        expect(t2.running());
    });


    // =========================================================================
    // Timeout - zero delay expires immediately
    //
    describe("timeout zero delay", []() {
        Timeout t(0, true);
        expect(t.expired());
    });


    // =========================================================================
    // TimedToken
    //
    describe("timed token", []() {
        TimedToken token("abc123", 100);
        expect(token.id() == "abc123");
        expect(token.delay() == 100);
        expect(token.running()); // constructor starts it

        expect(token == std::string("abc123"));
        expect(token == TimedToken("abc123"));

        TimedToken other("xyz", 100);
        expect(!(token == other));
    });


    // =========================================================================
    // TimedToken - expiry
    //
    describe("timed token expiry", []() {
        TimedToken token("sess", 50);
        expect(!token.expired());

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        expect(token.expired());
    });


    // =========================================================================
    // RateLimiter - allows initial burst
    //
    describe("rate limiter initial burst", []() {
        RateLimiter limiter(5.0, 6.0);

        // Should allow at least 5 messages initially
        for (int i = 0; i < 5; i++) {
            expect(limiter.canSend());
        }
    });


    // =========================================================================
    // RateLimiter - throttles excess
    //
    describe("rate limiter throttle", []() {
        RateLimiter limiter(3.0, 6.0);

        // Exhaust allowance
        for (int i = 0; i < 3; i++) {
            limiter.canSend();
        }

        // Next should be rejected (no time has passed)
        expect(!limiter.canSend());
    });


    // =========================================================================
    // RateLimiter - recovers over time
    //
    describe("rate limiter recovery", []() {
        RateLimiter limiter(2.0, 1.0); // 2 per second

        // Exhaust
        limiter.canSend();
        limiter.canSend();
        expect(!limiter.canSend());

        // Wait for recovery
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        expect(limiter.canSend());
    });


    // =========================================================================
    // IRegistry - register and create
    //
    describe("iregistry register and create", []() {
        IRegistry<Animal> registry;

        registry.registerType<Dog>("Dog");
        registry.registerType<Cat>("Cat");

        auto dog = registry.createInstance("Dog");
        expect(dog != nullptr);
        expect(dog->speak() == "woof");
        delete dog;

        auto cat = registry.createInstance("Cat");
        expect(cat != nullptr);
        expect(cat->speak() == "meow");
        delete cat;
    });


    // =========================================================================
    // IRegistry - unknown type returns null
    //
    describe("iregistry unknown type", []() {
        IRegistry<Animal> registry;
        auto result = registry.createInstance("Fish");
        expect(result == nullptr);
    });


    // =========================================================================
    // IRegistry - unregister
    //
    describe("iregistry unregister", []() {
        IRegistry<Animal> registry;
        registry.registerType<Dog>("Dog");

        expect(registry.createInstance("Dog") != nullptr);

        registry.unregisterType("Dog");
        expect(registry.createInstance("Dog") == nullptr);
    });


    // =========================================================================
    // IRegistry - types map
    //
    describe("iregistry types", []() {
        IRegistry<Animal> registry;
        registry.registerType<Dog>("Dog");
        registry.registerType<Cat>("Cat");

        auto types = registry.types();
        expect(types.size() == 2);
        expect(types.find("Dog") != types.end());
        expect(types.find("Cat") != types.end());
    });


    // =========================================================================
    // IRegistry - signals
    //
    describe("iregistry signals", []() {
        IRegistry<Animal> registry;
        std::string registered;
        std::string unregistered;

        registry.TypeRegistered += [&](const std::string& name) {
            registered = name;
        };
        registry.TypeUnregistered += [&](const std::string& name) {
            unregistered = name;
        };

        registry.registerType<Dog>("Dog");
        expect(registered == "Dog");

        registry.unregisterType("Dog");
        expect(unregistered == "Dog");
    });


    // =========================================================================
    // IRegistry - unregister nonexistent is safe
    //
    describe("iregistry unregister nonexistent", []() {
        IRegistry<Animal> registry;
        registry.unregisterType("DoesNotExist"); // should not crash
    });


    test::runAll();
    return test::finalize();
}
