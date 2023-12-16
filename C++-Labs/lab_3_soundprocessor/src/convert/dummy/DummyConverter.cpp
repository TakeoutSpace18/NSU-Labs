#include "DummyConverter.h"

#include <fmt/core.h>

#include "audio/AudioInput.h"
#include "audio/AudioOutput.h"

std::unique_ptr<Converter> DummyConverterCreator::create(const std::string& args)
{
    return std::make_unique<DummyConverter>();
}

void DummyConverterCreator::printDescription(std::ostream& os)
{
    os << "Dummy converter\n\tdoes nothing\n";
}

std::string DummyConverterCreator::name() const
{
    return "dummy";
}

void DummyConverter::apply(std::unique_ptr<AudioInput> input, std::unique_ptr<AudioOutput> output)
{
    while (input->hasData())
    {
        output->writeNextSamplesChunk(input->readNextSamplesChunk());
    }
}
