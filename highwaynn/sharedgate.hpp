#ifndef SHAREDGATE_HPP
#define SHAREDGATE_HPP

class SSiHighwayNeuron;
#include "../nnhelpers/ssctrainableparameter.hpp"
#include "../nnhelpers/sscactivation.hpp"
#include "blas/blasvector.hpp"

#include <QSharedPointer>
#include <QMap>

class SScSharedGate : public QVector<SScTrainableParameter*>
{
public:
    explicit SScSharedGate(QVector<SSiHighwayNeuron*> parents);
    virtual ~SScSharedGate();

    virtual void clear() { priv_clear(); QVector<SScTrainableParameter*>::clear(); }
    virtual QVector<double> net();
    /*!
     * \brief Called when a new input/target pattern was set - outputs of incoming layer have to be calculated
     */
    virtual void reset();
    /*!
     * \brief Training cycle ended - update weights
     */
    virtual void endOfCycle();

    QVariantMap toVM() const;

    bool fromVM(const QVariantMap& vm);

private:
    void priv_clear();
    QVector<SSiHighwayNeuron*>  m_parents;
    bool                        m_dirty;
    QVector<double>             m_net;
};

#endif // SHAREDGATE_HPP
