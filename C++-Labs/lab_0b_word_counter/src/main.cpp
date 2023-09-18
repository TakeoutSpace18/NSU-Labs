#include <cstdlib>
#include <iostream>

#include "WordCounter.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Wrong arguments" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        WordCounter::WriteFileWordFrequencyToCSV(argv[1], argv[2]);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}