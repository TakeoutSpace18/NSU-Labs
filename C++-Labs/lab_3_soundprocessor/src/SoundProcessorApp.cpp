#include "SoundProcessorApp.h"

#include <iostream>

#include "utils/CommandLineOptions.h"

int SoundProcessorApp::launch(const CommandLineOptions& cmdOptions)
{
    if (cmdOptions.printHelpIfRequired(std::cout)) {
        return EXIT_SUCCESS;
    }

    auto output = cmdOptions.getOutputFileName();
    auto input = cmdOptions.getInputFileNames();

    return EXIT_SUCCESS;
}
