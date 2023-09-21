#include <cstdlib>
#include <iostream>

#include "WordCounter.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Wrong arguments" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        std::locale::global(std::locale("ru_RU.utf8").combine<std::numpunct<wchar_t>>(std::locale("C")));
        WordCounter::WriteFileWordFrequencyToCSV(argv[1], argv[2]);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}