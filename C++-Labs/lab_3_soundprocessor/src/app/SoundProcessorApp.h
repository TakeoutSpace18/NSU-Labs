#ifndef SOUNDPROCESSORAPP_H
#define SOUNDPROCESSORAPP_H
#include <app/CommandLineOptions.h>

#include "audio/AudioFile.h"


class SoundProcessorApp {
public:
    int launchPipeline(const CommandLineOptions& cmdArgs);
};



#endif //SOUNDPROCESSORAPP_H
