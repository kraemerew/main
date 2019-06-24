#ifndef HWNEURON_HPP
#define HWNEURON_HPP

#include "sscneuron.hpp"
#include <QMap>
#include <QSharedPointer>


class SScHighwayNetwork;
class SScHighwayNeuron : public SScNeuron
{
public:
    /*!
     * \brief Constructor
     * \param nw    Containing network
     * \param layer Layer
     * \param nr    Index in layer
     */
    explicit SScHighwayNeuron(SScHighwayNetwork* nw, int layer, int nr);

    virtual bool  setInput(double) { Q_ASSERT(false); return false; }
    virtual bool  setTarget(double) { Q_ASSERT(false); return false; }

    virtual bool addInput(SScNeuron* other, double v);
    virtual bool delInput(SScNeuron* other);
    virtual double net();
    virtual double deltaw(SScNeuron*);
    virtual double deltag();
    virtual double out();

    virtual QList<SScNeuron*> inputs() const { return m_in.keys(); }

    virtual bool trainingStep(bool cycleDone);

protected:
    SScHighwayNetwork* m_nw;
    int m_layer, m_nr;
    QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> > m_in;

};
#endif // HWNEURON_HPP
