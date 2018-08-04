#ifndef SSCNEURON_HPP
#define SSCNEURON_HPP

#include <QMap>
#include <QVariantMap>

#include "sscweight.hpp"

class SScNeuron
{
public:
    SScNeuron();
    bool addInput(SScNeuron* other, double v);
    bool delInput(SScNeuron* other);
    double out() const;
    double net() const;
    QMap<SScNeuron*,double>   m_in;
};

#endif // SSCNEURON_HPP
