#include "icy/archo/zipfile.h"
#include "icy/test.h"
#include "icy/filesystem.h"
#include "icy/platform.h"


using namespace icy;
using namespace icy::test;


std::string testDataDir(const std::string& file)
{
    std::string dir;
    fs::addnode(dir, ICY_SOURCE_DIR);
    fs::addnode(dir, "archo");
    fs::addnode(dir, "tests");
    fs::addnode(dir, "data");
    if (!file.empty())
        fs::addnode(dir, file);
    return dir;
}


int main(int argc, char** argv)
{
    test::init();

    describe("extract zip archive", []() {
        std::string path(testDataDir("test.zip"));
        archo::ZipFile zip(path);

        std::string output(icy::getCwd());
        fs::addnode(output, "archo");
        zip.extract(output);

        fs::addnode(output, "HELLO");
        expect(fs::exists(output));
        fs::unlink(output);
    });


    describe("zip file open/close lifecycle", []() {
        archo::ZipFile zip;
        expect(!zip.opened());

        std::string path(testDataDir("test.zip"));
        zip.open(path);
        expect(zip.opened());

        zip.close();
        expect(!zip.opened());
    });


    describe("zip file enumerate entries", []() {
        std::string path(testDataDir("test.zip"));
        archo::ZipFile zip(path);
        expect(zip.opened());

        int count = 0;
        if (zip.goToFirstFile()) {
            do {
                std::string name = zip.currentFileName();
                expect(!name.empty());
                count++;
            } while (zip.goToNextFile());
        }
        expect(count > 0);
    });


    describe("zip file info populated on open", []() {
        std::string path(testDataDir("test.zip"));
        archo::ZipFile zip(path);

        expect(!zip.info.empty());
        for (const auto& fi : zip.info) {
            expect(!fi.path.empty());
        }
    });


    describe("zip file invalid path throws", []() {
        try {
            archo::ZipFile zip("/nonexistent/path/fake.zip");
            expect(0 && "opening nonexistent zip - must throw");
        } catch (std::exception&) {
        }
    });

    test::runAll();
    return test::finalize();
}
