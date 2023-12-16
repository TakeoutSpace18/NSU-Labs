#include "SoundProcessorApp.h"

#include <iostream>
#include <ranges>
#include <fmt/ranges.h>

#include "app/ConfigurationFile.h"
#include "app/InputFilesLoader.h"
#include "app/CommandLineOptions.h"
#include "convert/Converter.h"
#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"
#include "convert/ConverterFactory.h"

int SoundProcessorApp::launch(const CommandLineOptions& cmdOptions)
{
    if (cmdOptions.printHelpIfRequired(std::cout)) {
        return EXIT_SUCCESS;
    }

    InputFilesLoader input_files_loader(cmdOptions.getInputFileNames());


    ConfigurationFile config(cmdOptions.getConfigurationFileName());

    std::string tmp_filename = "tmp.wav";
    auto initial_file = input_files_loader.getByIndex(0);
    for (auto [name, args] : config)
    {
        auto tmp_file = initial_file->copyWithoutDataTo(tmp_filename);
        auto converter = ConverterFactory::create(name, args);
        converter->apply(initial_file->beginReading(), tmp_file->beginWriting());
    }


    return EXIT_SUCCESS;
}
