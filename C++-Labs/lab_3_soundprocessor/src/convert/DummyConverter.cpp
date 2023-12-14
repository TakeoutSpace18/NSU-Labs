#include "DummyConverter.h"

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

DummyConverter::DummyConverter(const ConfigurationFile::ConverterArgs& args)
{

}

void DummyConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    while (input->hasData())
    {
        output->writeNextChannels(input->readNextChannels());
    }
}
