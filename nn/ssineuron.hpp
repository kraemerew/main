#ifndef SSINEURON_HPP
#define SSINEURON_HPP

#include <QtMath>

class SSiNeuron
{
public:
    inline  double perr() { return qPow(err(),2.0); }
    virtual double err() { return 0.0; }
    virtual double out() = 0;
    virtual double net() = 0;
};

#endif // SSINEURON_HPP
