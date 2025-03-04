#include <QApplication>

#include <qcoreapplication.h>
#include <random>
#include <memory>
#include <chrono>

#include "Application.h"
#include "WaveEquation.h"

#define RANDOM_SOURCE_LOCATION 0

int main(int argc, char* argv[])
{
    WaveEquation::AreaParams area = {
        .xa = 0.0,
        .xb = 4.0,
        .ya = 0.0,
        .yb = 4.0,
        .nx = 900,
        .ny = 900
    };

    Utils::Vec2i source;

    if (RANDOM_SOURCE_LOCATION) {
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_int_distribution<int> distr_x(0, area.nx);
        std::uniform_int_distribution<int> distr_y(0, area.ny);

        source = {
            .x = distr_x(eng),
            .y = distr_y(eng)
        };
    }
    else {
        source = {
            .x = area.nx * 3 / 4,
            .y = area.ny * 3 / 4
        };
    }

    auto equation = std::make_unique<WaveEquation>(area, source);

    if (argc == 2 && strcmp(argv[1], "--gui") == 0) {
        QApplication app(argc, argv);
        Application window(std::move(equation));
        window.show();
        return app.exec();
    }

    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    equation->skipNIterarions(1500);
    auto end = high_resolution_clock::now();

    uint64_t elapsed = duration_cast<milliseconds>(end - start).count();
    std::cout << "time: " << elapsed << "\n";

    WaveEquation::Output out = equation->getCurrentState();
    size_t size = area.nx * area.ny * sizeof(WaveEquation::ValueType);
    Utils::writeToFile("out.dat", reinterpret_cast<const char*>(out.data), size);
}
