#include "scy/av/devicemanager.h"
#include "scy/logger.h"


using namespace scy;


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    {
        av::DeviceManager devman;
        devman.print(std::cout);

        std::cout << "Input formats:\n\t";
        av::printInputFormats(std::cout);
        std::cout << std::endl;

        std::cout << "Output formats:\n\t";
        av::printOutputFormats(std::cout);
        std::cout << std::endl;
    }
    Logger::destroy();
    return 0;
}
