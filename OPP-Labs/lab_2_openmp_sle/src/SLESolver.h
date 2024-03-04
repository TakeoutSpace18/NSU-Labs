#ifndef SLESOLVER_H
#define SLESOLVER_H

// this helps to utilize intel P-cores while E-cores are still busy
#define SCHEDULE schedule(dynamic, 1)

// however, best perfomance can be achived using only P-cores and static scheduling
// #define SCHEDULE schedule(static)

class SLESolver {
public:
    using DataType = float;

protected:
    SLESolver() = default;

    static constexpr DataType sParam = -0.01;
};


#endif //SLESOLVER_H
