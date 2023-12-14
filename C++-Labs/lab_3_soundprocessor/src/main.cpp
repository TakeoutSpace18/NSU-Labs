#include "utils/Logger.h"

#include "app/SoundProcessorApp.h"
#include "app/CommandLineOptions.h"

int main(int argc, char** argv) {
    try {
        CommandLineOptions cmdOptions(argc, argv);
        SoundProcessorApp app;
        return app.launch(cmdOptions);
    }
    catch (const std::exception& e) {
        logger << Logger::Error << e.what() << '\n';
        return EXIT_FAILURE;
    }

}
