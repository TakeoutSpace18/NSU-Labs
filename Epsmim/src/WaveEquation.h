#ifndef WAVE_EQUATION_H
#define WAVE_EQUATION_H

#include <vector>

#include "Utils.h"

class WaveEquation
{
public:
    using ValueType = float;

    struct AreaParams
    {
        // modeling area bounds
        ValueType xa;
        ValueType xb;
        ValueType ya;
        ValueType yb;

        // grid resolution
        int nx;
        int ny;
    };

    struct Output
    {
        const ValueType *data;
        ValueType max;
    };

    WaveEquation(const AreaParams& area, const Utils::Vec2i& source);

    Output nextIteration();
    Output skipNIterarions(int n);
    Output getCurrentState();

    int nx() const { return m_area.nx; };
    int ny() const { return m_area.ny; };

private:
    ValueType sourceFunc(int n);

    void generatePhaseSpeed();

    std::vector<ValueType> m_data;
    std::vector<ValueType> m_phaseSpeed;

    ValueType *m_buf1;
    ValueType *m_buf2;

    AreaParams m_area;
    Utils::Vec2i m_source;
    Utils::Vec2<ValueType> m_gridStep;
    Utils::Vec2<ValueType> m_stepInvariant;
    ValueType m_tau;
    ValueType m_max;
    int m_step;
};

#endif
