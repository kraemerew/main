#ifndef SSCHWGATE_HPP
#define SSCHWGATE_HPP

#include "../nnhelpers/ssctrainableparameter.hpp"
#include "../nnhelpers/sscactivation.hpp"
#include <QSharedPointer>
#include <QMap>

class SSiHighwayNeuron;

class SScHighwayGate : public QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >
{
public:
    explicit SScHighwayGate(SSiHighwayNeuron* parent);
    virtual ~SScHighwayGate();

    virtual bool addInput(SSiHighwayNeuron *other, double v);
    virtual bool delInput(SSiHighwayNeuron *other);
    virtual double net();

private:
    SSiHighwayNeuron*  m_parent;
};

#endif // SSCHWGATE_HPP
