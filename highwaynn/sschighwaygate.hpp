#ifndef SSCHWGATE_HPP
#define SSCHWGATE_HPP

class SScHighwayNetwork;
#include "../nnhelpers/ssctrainableparameter.hpp"
#include "../nnhelpers/sscactivation.hpp"
#include "blas/blasvector.hpp"

#include <QSharedPointer>
#include <QMap>

class SSiHighwayNeuron;

class SScHighwayGate : public QMap<SSiHighwayNeuron*,QSharedPointer<SScTrainableParameter> >
{
public:
    explicit SScHighwayGate(SSiHighwayNeuron* parent);
    virtual ~SScHighwayGate();

    virtual bool addInput(SSiHighwayNeuron *other, double v, SScTrainableParameter::Type t);
    virtual bool addInput(SSiHighwayNeuron *other, SScTrainableParameter* tp);
    virtual bool delInput(SSiHighwayNeuron *other);
    virtual double net();
    /*!
     * \brief Called when a new input/target pattern was set - outputs of incoming layer have to be calculated
     */
    virtual void reset();
    /*!
     * \brief Training cycle ended - update weights
     */
    virtual void endOfCycle();

    QVariantMap toVM() const;

    bool fromVM(SScHighwayNetwork*,const QVariantMap& vm);

private:
    SSiHighwayNeuron*   m_parent;
    bool                m_dirty;
    double              m_net;        
};

#endif // SSCHWGATE_HPP
