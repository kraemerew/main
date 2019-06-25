#ifndef SSCGATE_HPP
#define SSCGATE_HPP

#include "ssctrainableparameter.hpp"
#include "sscactivation.hpp"
#include <QSharedPointer>
#include <QMap>

class SScNeuron;

class SScGate : public QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> >
{
public:
    explicit SScGate(SScNeuron* parent);
    virtual ~SScGate();

    virtual bool addInput(SScNeuron *other, double v);
    virtual bool delInput(SScNeuron *other);
    virtual double net();

private:
    SScNeuron*  m_parent;    
};

#endif // SSCGATE_HPP
