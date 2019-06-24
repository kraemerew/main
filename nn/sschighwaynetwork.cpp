#include "sschighwaynetwork.hpp"
#include "sschighwayneuron.hpp"

SScHighwayNetwork::SScHighwayNetwork(int inr, int onr)
    : m_lcnt(0),
      m_bias(SScNeuron::create(SScNeuron::NeuronType_Bias))
{
    Q_ASSERT(inr>0);
    Q_ASSERT(onr>0);
    Q_CHECK_PTR(m_bias);
    QList<SScNeuron*> il, ol;
    for (int i=0; i<inr; ++i) il << newInputNeuron ();
    for (int i=0; i<onr; ++i) ol << newOutputNeuron();
    m_layers[-1]=il;
    m_layers[-2]=ol;
}

SScHighwayNetwork::~SScHighwayNetwork()
{
    foreach (SScNeuron* n, allNeurons()) delete n;
}

QList<SScNeuron*> SScHighwayNetwork::layer(int nr) const
{
    QList<SScNeuron*> ret;
    if (m_layers.contains(nr)) ret = m_layers[nr];
    return ret;
}

int SScHighwayNetwork::addHiddenLayer   (int nr)
{
    QList<SScNeuron*> l;
    for (int i=0; i<nr; ++i) l << newHiddenNeuron();
    m_layers[m_lcnt]=l;
    ++m_lcnt;
    return m_lcnt-1;
}

int SScHighwayNetwork::addHighwayLayer()
{
    QList<SScNeuron*> l;
    const int nr = layerSize(m_lcnt-1); //< will be the size of the last layer or input layer if no layers were added
    for (int i=0; i<nr; ++i) l << newHighwayNeuron(m_lcnt,i);
    m_trans[m_lcnt] = newTransformNeuron();
    m_layers[m_lcnt]=l;
    ++m_lcnt;
    return m_lcnt-1;
}

SScNeuron* SScHighwayNetwork::newHighwayNeuron(int l, int nr)
{
    return new SScHighwayNeuron(this,l,nr);
}

SScNeuron* SScHighwayNetwork::newTransformNeuron()
{
    SScNeuron* n = SScNeuron::create(SScNeuron::NeuronType_Hidden);
    n->setActivation(SScActivation::ACT_SIGMOID);
    return n;
}

SScNeuron* SScHighwayNetwork::neuron(int l, int nr) const
{
    Q_ASSERT(nr>=0);
    const QList<SScNeuron*> lay = layer(l);
    if (lay.size()>nr) return lay[nr];
    return NULL;
}

QList<SScNeuron*> SScHighwayNetwork::allNeurons() const
{
    QList<SScNeuron*> ret;
    ret << m_bias;
    ret << m_trans.values();
    for (int i=0; i<layers(); ++i) ret << layer(i);
    return ret;
}

void SScHighwayNetwork::fullyConnect(int l, double min, double max)
{
    QList<SScNeuron*> froml = layer(l-1), tol = layer(l);
    froml << biasNeuron();
    if (m_trans.contains(l)) tol << m_trans[l];


    foreach(SScNeuron* to, tol) foreach(SScNeuron* from, froml)
    {
        const double v = min+(max-min)*((double)qrand()/(double)RAND_MAX);
        to->addInput(from,v);
    }
}
