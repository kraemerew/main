#ifndef HIGHWAYNETWORK_HPP
#define HIGHWAYNETWORK_HPP

#include "sscneuron.hpp"
#include "sscvm.hpp"

#include <QMap>
class SScHighwayNetwork
{
public:
    explicit SScHighwayNetwork(int inr, int onr);
    virtual ~SScHighwayNetwork();

    int addHiddenLayer (int nr);
    int addHighwayLayer();

    inline int layerSize    (int nr) const { return m_layers.contains(nr) ? m_layers[nr].size() : -1; }
    inline int inputSize    () const { return layerSize(-1); }
    inline int outputSize   () const { return layerSize(-2); }
    inline int layers       () const { return m_lcnt; }
    inline QList<SScNeuron*> inputLayer () const { return layer(-1); }
    inline QList<SScNeuron*> outputLayer() const { return layer(-2); }
    QList<SScNeuron*> layer(int nr) const;
    inline SScNeuron* biasNeuron() const { return m_bias; }
    inline SScNeuron* transformNeuron(int l) const { Q_ASSERT(m_trans.contains(l)); return m_trans.contains(l) ? m_trans[l] : NULL; }
    SScNeuron* neuron(int l, int nr) const;
    void fullyConnect(int l, double min, double max);

private:

    QList<SScNeuron*> allNeurons() const;
    inline SScNeuron* newHiddenNeuron  () { return SScNeuron::create(SScNeuron::NeuronType_Hidden); }
    inline SScNeuron* newInputNeuron   () { return SScNeuron::create(SScNeuron::NeuronType_Input); }
    inline SScNeuron* newOutputNeuron  () { return SScNeuron::create(SScNeuron::NeuronType_Output); }
           SScNeuron* newHighwayNeuron (int l, int nr);
           SScNeuron* newTransformNeuron();

    int m_lcnt;
    QMap<int,QList<SScNeuron*> > m_layers;
    QMap<int,SScNeuron*>    m_trans;
    SScNeuron* m_bias;
};

#endif // HIGHWAYNETWORK_HPP
