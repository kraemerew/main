#ifndef SSCACTIVATION_HPP
#define SSCACTIVATION_HPP

#include <QString>

class SScActivation
{
enum SSeActivation
{
    Act_Sigmoid,
    Act_Tanh
};

public:
    virtual double activate(double net) = 0;
    virtual double fn() const = 0;
    virtual double dev() const = 0;

    static SScActivation* create(SSeActivation);;
};



#endif // SSCACTIVATION_HPP
