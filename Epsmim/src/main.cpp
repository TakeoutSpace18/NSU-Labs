#include <QApplication>

#include <qcoreapplication.h>
#include <random>
#include <memory>

#include "Application.h"
#include "WaveEquation.h"

#define RANDOM_SOURCE_LOCATION 0

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

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
    Application window(std::move(equation));
    window.show();

    // exit after 30 seconds
    QTimer::singleShot(30 * 1000, []() {
        QCoreApplication::quit();
    });

    return app.exec();
}
