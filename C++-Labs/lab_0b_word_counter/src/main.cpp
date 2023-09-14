#include <cstdlib>
#include <iostream>

#include "WordCounter.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Wrong arguments" << std::endl;
        return EXIT_FAILURE;
    }

    WordCounter::WriteFileWordFrequencyToCSV(argv[1], argv[2]);
    return EXIT_SUCCESS;
}