#include "DummyConverter.h"

#include <fmt/core.h>

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

DummyConverter::DummyConverter(const std::string& args)
{

}

void DummyConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    while (input->hasData())
    {
        output->writeNextSamplesChunk(input->readNextSamplesChunk());
    }
}

void DummyConverter::printDescription(std::ostream& os)
{
    os << "Dummy converter: does nothing\n";
}
