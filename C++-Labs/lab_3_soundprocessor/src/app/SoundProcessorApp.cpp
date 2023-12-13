#include "SoundProcessorApp.h"

#include <iostream>

#include "ConfigurationFile.h"
#include "InputFilesLoader.h"
#include "audio/Pcm_s16le_AudioFile.h"
#include "CommandLineOptions.h"

int SoundProcessorApp::launch(const CommandLineOptions& cmdOptions)
{
    if (cmdOptions.printHelpIfRequired(std::cout)) {
        return EXIT_SUCCESS;
    }

    InputFilesLoader input_files_loader(cmdOptions.getInputFileNames());

    auto audio_file = input_files_loader.getByIndex(0);

    ConfigurationFile config(cmdOptions.getConfigurationFileName());
    for (auto [name, args] : config)
    {

    }


    return EXIT_SUCCESS;
}
