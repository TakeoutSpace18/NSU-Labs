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

int SoundProcessorApp::launchPipeline(const CommandLineOptions& cmdOptions)
{
    if (cmdOptions.printHelpIfRequired(std::cout))
    {
        return EXIT_SUCCESS;
    }

    InputFilesLoader input_files_loader(cmdOptions.getInputFileNames());
    fs::path temp_directory = "soundprocessor_temp";
    fs::create_directory(temp_directory);

    ConfigurationFile config(cmdOptions.getConfigurationFileName());
    auto input_file = input_files_loader.getByIndex(0);

    std::size_t stage_count = 0;
    for (auto [name, args]: config)
    {
        ++stage_count;
        std::shared_ptr<AudioFile> output_file = input_file->copyWithoutDataTo(
            temp_directory / fmt::format("tmp_{}.wav", stage_count));

        auto converter = ConverterFactory::create(name, args, input_files_loader);
        converter->apply(input_file->beginReading(), output_file->beginWriting());

        std::swap(input_file, output_file);
    }

    fs::rename(temp_directory / fmt::format("tmp_{}.wav", stage_count), cmdOptions.getOutputFileName());

    return EXIT_SUCCESS;
}
