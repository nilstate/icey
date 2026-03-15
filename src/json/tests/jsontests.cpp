#include "scy/base.h"
#include "scy/filesystem.h"
#include "scy/json/configuration.h"
#include "scy/json/iserializable.h"
#include "scy/json/json.h"
#include "scy/test.h"

#include <fstream>
#include <sstream>


using namespace std;
using namespace scy;
using namespace scy::test;


// Helper: create a temp JSON file and return its path
static std::string writeTempJson(const std::string& content)
{
    std::string path = fs::makePath(SCY_SOURCE_DIR, "..");
    fs::addnode(path, "json_test_tmp.json");
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
    return path;
}

static void removeTempJson(const std::string& path)
{
    std::remove(path.c_str());
}


// =============================================================================
// ISerializable test object
//
struct TestObject : public json::ISerializable
{
    std::string name;
    int value;

    TestObject(const std::string& n = "", int v = 0)
        : name(n)
        , value(v)
    {}

    void serialize(json::value& root) override
    {
        root["name"] = name;
        root["value"] = value;
    }

    void deserialize(json::value& root) override
    {
        name = root["name"].get<std::string>();
        value = root["value"].get<int>();
    }
};


int main(int argc, char** argv)
{
    test::init();

    // =========================================================================
    // json::value basics
    //
    describe("json value basics", []() {
        json::value j;
        j["name"] = "test";
        j["count"] = 42;
        j["active"] = true;
        j["tags"] = {"a", "b", "c"};

        expect(j["name"] == "test");
        expect(j["count"] == 42);
        expect(j["active"] == true);
        expect(j["tags"].size() == 3);
        expect(j["tags"][0] == "a");
    });


    // =========================================================================
    // json::value parse and dump
    //
    describe("json parse and dump", []() {
        std::string input = R"({"key":"value","num":123})";
        json::value j = json::value::parse(input);

        expect(j["key"] == "value");
        expect(j["num"] == 123);

        std::string output = j.dump();
        expect(output.find("key") != std::string::npos);
        expect(output.find("value") != std::string::npos);
    });


    // =========================================================================
    // assertMember
    //
    describe("assertMember", []() {
        json::value j;
        j["exists"] = "yes";

        // Should not throw
        json::assertMember(j, "exists");

        // Should throw for missing member
        bool threw = false;
        try {
            json::assertMember(j, "missing");
        } catch (const std::runtime_error& e) {
            threw = true;
            expect(std::string(e.what()).find("missing") != std::string::npos);
        }
        expect(threw);
    });


    // =========================================================================
    // loadFile / saveFile round-trip
    //
    describe("loadFile and saveFile", []() {
        json::value original;
        original["name"] = "libsourcey";
        original["version"] = 2;
        original["features"] = {"networking", "crypto", "av"};

        std::string path = writeTempJson("{}"); // placeholder

        // Save
        json::saveFile(path, original, 2);

        // Load back
        json::value loaded;
        json::loadFile(path, loaded);

        expect(loaded["name"] == "libsourcey");
        expect(loaded["version"] == 2);
        expect(loaded["features"].size() == 3);
        expect(loaded["features"][1] == "crypto");

        removeTempJson(path);
    });


    // =========================================================================
    // loadFile nonexistent throws
    //
    describe("loadFile nonexistent throws", []() {
        json::value j;
        bool threw = false;
        try {
            json::loadFile("/nonexistent/path/file.json", j);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw);
    });


    // =========================================================================
    // saveFile with no indent (compact)
    //
    describe("saveFile compact", []() {
        json::value j;
        j["a"] = 1;

        std::string path = writeTempJson("{}");
        json::saveFile(path, j, 0);

        // Read raw file content - should have no newlines in the JSON body
        std::ifstream ifs(path);
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
        expect(content.find('\n') == std::string::npos);

        removeTempJson(path);
    });


    // =========================================================================
    // countNestedKeys
    //
    describe("countNestedKeys", []() {
        json::value j = json::value::parse(R"({
            "items": [
                {"type": "a", "nested": {"type": "b"}},
                {"type": "c"},
                {"other": "d"}
            ]
        })");

        int count = 0;
        json::countNestedKeys(j, "type", count);
        expect(count == 3); // three objects have "type"
    });


    // =========================================================================
    // hasNestedKey
    //
    describe("hasNestedKey", []() {
        json::value j = json::value::parse(R"({
            "level1": {
                "level2": {
                    "target": "found"
                }
            }
        })");

        expect(json::hasNestedKey(j, "target"));
        expect(!json::hasNestedKey(j, "nonexistent"));
    });


    // =========================================================================
    // findNestedObjectWithProperty
    //
    describe("findNestedObjectWithProperty", []() {
        json::value j = json::value::parse(R"({
            "items": [
                {"name": "alpha", "id": 1},
                {"name": "beta", "id": 2},
                {"name": "gamma", "id": 3}
            ]
        })");

        json::value* result = nullptr;
        bool found = json::findNestedObjectWithProperty(j, result, "", "beta");
        expect(found);
        expect(result != nullptr);
        expect((*result)["id"] == 2);
    });


    // =========================================================================
    // findNestedObjectWithProperty - not found
    //
    describe("findNestedObjectWithProperty not found", []() {
        json::value j = json::value::parse(R"({"items": [{"name": "x"}]})");

        json::value* result = nullptr;
        bool found = json::findNestedObjectWithProperty(j, result, "", "zzz");
        expect(!found);
    });


    // =========================================================================
    // ISerializable
    //
    describe("iserializable serialize", []() {
        TestObject obj("hello", 42);

        std::string output;
        bool ok = json::serialize(&obj, output);
        expect(ok);
        expect(output.find("hello") != std::string::npos);
        expect(output.find("42") != std::string::npos);
    });


    // =========================================================================
    // ISerializable deserialize
    //
    describe("iserializable deserialize", []() {
        TestObject obj;
        std::string input = R"({"name":"world","value":99})";

        bool ok = json::deserialize(&obj, input);
        expect(ok);
        expect(obj.name == "world");
        expect(obj.value == 99);
    });


    // =========================================================================
    // ISerializable round-trip
    //
    describe("iserializable round-trip", []() {
        TestObject original("roundtrip", 777);

        std::string serialized;
        json::serialize(&original, serialized);

        TestObject restored;
        json::deserialize(&restored, serialized);

        expect(restored.name == original.name);
        expect(restored.value == original.value);
    });


    // =========================================================================
    // ISerializable null pointer
    //
    describe("iserializable null pointer", []() {
        std::string output;
        expect(!json::serialize(nullptr, output));

        std::string input = "{}";
        expect(!json::deserialize(nullptr, input));
    });


    // =========================================================================
    // ISerializable invalid JSON
    //
    describe("iserializable invalid json", []() {
        TestObject obj;
        std::string bad = "not valid json {{{";
        bool ok = json::deserialize(&obj, bad);
        expect(!ok);
    });


    // =========================================================================
    // Configuration - set/get via base class API
    //
    describe("configuration set and get", []() {
        std::string path = writeTempJson("{}");

        json::Configuration config;
        config.load(path, true);

        config.setString("name", "test");
        config.setInt("port", 8080);
        config.setBool("debug", true);
        config.setDouble("ratio", 3.14);

        expect(config.getString("name") == "test");
        expect(config.getInt("port") == 8080);
        expect(config.getBool("debug") == true);
        expect(config.getDouble("ratio") > 3.13);
        expect(config.getDouble("ratio") < 3.15);

        expect(config.loaded());

        removeTempJson(path);
    });


    // =========================================================================
    // Configuration - save and reload
    //
    describe("configuration save and reload", []() {
        std::string path = writeTempJson("{}");

        // Write config
        {
            json::Configuration config;
            config.load(path, true);
            config.setString("app.name", "libsourcey");
            config.setInt("app.version", 2);
            config.save();
        }

        // Reload in fresh instance
        {
            json::Configuration config;
            config.load(path);
            expect(config.getString("app.name") == "libsourcey");
            expect(config.getInt("app.version") == 2);
        }

        removeTempJson(path);
    });


    // =========================================================================
    // Configuration - remove
    //
    describe("configuration remove", []() {
        std::string path = writeTempJson("{}");

        json::Configuration config;
        config.load(path, true);
        config.setString("keep", "yes");
        config.setString("remove_me", "bye");

        expect(config.exists("remove_me"));
        config.remove("remove_me");
        expect(!config.exists("remove_me"));
        expect(config.exists("keep"));

        removeTempJson(path);
    });


    // =========================================================================
    // Configuration - keys
    //
    describe("configuration keys", []() {
        std::string path = writeTempJson("{}");

        json::Configuration config;
        config.load(path, true);
        config.setString("a", "1");
        config.setString("b", "2");
        config.setString("c", "3");

        std::vector<std::string> keys;
        config.keys(keys);
        expect(keys.size() == 3);

        removeTempJson(path);
    });


    // =========================================================================
    // Configuration - default values
    //
    describe("configuration defaults", []() {
        std::string path = writeTempJson("{}");

        json::Configuration config;
        config.load(path, true);

        expect(config.getString("missing", "fallback") == "fallback");
        expect(config.getInt("missing", 42) == 42);
        expect(config.getBool("missing", true) == true);
        expect(config.getDouble("missing", 1.5) == 1.5);

        removeTempJson(path);
    });


    test::runAll();
    return test::finalize();
}
