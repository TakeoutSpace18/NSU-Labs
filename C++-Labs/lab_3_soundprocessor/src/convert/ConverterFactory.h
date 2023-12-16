#ifndef CONVERTERFACTORY_H
#define CONVERTERFACTORY_H
#include <memory>

#include "Converter.h"
#include "app/InputFilesLoader.h"


class ConverterFactory
{
public:
    static std::unique_ptr<Converter> create(const std::string& name,
                                             const std::string& args,
                                             InputFilesLoader& input_files);

    static void printDescriptions(std::ostream& os);

private:
    static std::map<const std::string, std::shared_ptr<AbstractConverterCreator>> m_creators;
};


#endif //CONVERTERFACTORY_H
