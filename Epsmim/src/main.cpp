#include <QApplication>

#include <random>
#include <memory>

#include "Application.h"
#include "WaveEquation.h"


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

    std::random_device rd;

    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr_x(0, area.nx);
    std::uniform_int_distribution<int> distr_y(0, area.ny);

    Utils::Vec2i source = {
        .x = distr_x(eng),
        .y = distr_y(eng)
    };

    auto equation = std::make_unique<WaveEquation>(area, source);
    Application window(std::move(equation));
    window.show();

    return app.exec();
}
