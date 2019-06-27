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
    virtual void reset();
private:
    SSiHighwayNeuron*  m_parent;
    bool                m_dirty;
    double  m_net;
};

#endif // SSCHWGATE_HPP
