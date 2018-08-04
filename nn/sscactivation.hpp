#ifndef SSCACTIVATION_HPP
#define SSCACTIVATION_HPP

#include <QString>

class SScActivation
{
public:
enum SSeActivation
{
    Act_Identity,
    Act_Logistic,
    Act_Tanh,
    Act_Rbf,
    Act_MHat,
    Act_Softmax
};

    virtual double activate(double net) = 0;
    virtual double fn() const = 0;
    virtual double dev() const = 0;

    static SScActivation* create(SSeActivation);
};

#endif // SSCACTIVATION_HPP
