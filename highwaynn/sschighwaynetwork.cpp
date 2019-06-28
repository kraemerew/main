#include "sschighwaynetwork.hpp"
#include "sschighwayneuron.hpp"
#include "../nnhelpers/ssccycledetector.hpp"
#include <QSet>

SScHighwayNetwork::SScHighwayNetwork()
{
}
SScHighwayNetwork::~SScHighwayNetwork()
{
    foreach(SSiHighwayNeuron*n, m_neurons) delete n;
    m_neurons.clear();
}

int SScHighwayNetwork::addNeuron   (SSiHighwayNeuron::SSeNeuronType type, const QString& name)
{
    m_neurons << SSiHighwayNeuron::create(type,name);
    return m_neurons.size()-1;
}

bool SScHighwayNetwork::delNeuron   (int idx)
{

    SSiHighwayNeuron* n = idx2n(idx);
    if (!n) return false;
    foreach(SSiHighwayNeuron* other, m_neurons) (void) disconnect(n,other);
    m_neurons.removeAt(idx);
    delete n;
    return true;
}

bool SScHighwayNetwork::disconnect  (SSiHighwayNeuron* from, SSiHighwayNeuron* to)
{
    if (!contains(from) || !contains(to)) return false;
    return to->delInput(from);
}

void SScHighwayNetwork::connectForward()
{
    QMap<SSiHighwayNeuron*,QSet<SSiHighwayNeuron*> > m; //< outputs for each neuron
    foreach(SSiHighwayNeuron* n, m_neurons) foreach(SSiHighwayNeuron* inp, n->inputs())
    {
        m[inp] << n;    //< neuron inp feeds neuron n
    }
    foreach(SSiHighwayNeuron* n, m_neurons) if (m.contains(n)) n->connectForward(m[n].toList());
}

bool SScHighwayNetwork::connect(SSiHighwayNeuron* from, SSiHighwayNeuron* to, double v)
{
    if (!contains(from) || !contains(to)) return false;
    to->addInput(from,v);
    if (!isFeedForward())
    {
        (void) disconnect(from,to);
        return false;
    }
    return true;
}

bool SScHighwayNetwork::isFeedForward() const
{
    SScCycleDetector cdt(m_neurons.size());
    foreach(SSiHighwayNeuron* to, m_neurons) foreach(SSiHighwayNeuron* from, to->inputs ()) cdt.addEdge(n2idx(from),n2idx(to));
    foreach(SSiHighwayNeuron* to, m_neurons) foreach(SSiHighwayNeuron* from, to->inputsC()) cdt.addEdge(n2idx(from),n2idx(to));
    return !cdt.isCyclic();
}

int                 SScHighwayNetwork::addInputNeuron   (const QString& name)           { return addNeuron(SSiHighwayNeuron::NeuronType_Input,  name); }
int                 SScHighwayNetwork::addHiddenNeuron  (const QString& name)           { return addNeuron(SSiHighwayNeuron::NeuronType_Hidden, name); }
int                 SScHighwayNetwork::addOutputNeuron  (const QString& name)           { return addNeuron(SSiHighwayNeuron::NeuronType_Output, name); }
int                 SScHighwayNetwork::addBiasNeuron    (const QString& name)           { return addNeuron(SSiHighwayNeuron::NeuronType_Bias, name); }
bool                SScHighwayNetwork::delNeuron        (SSiHighwayNeuron* n)           { return delNeuron(n2idx(n)); }
int                 SScHighwayNetwork::n2idx            (SSiHighwayNeuron* n) const     { return m_neurons.indexOf(n); }
SSiHighwayNeuron*   SScHighwayNetwork::idx2n            (int idx) const                 { if ((idx<0) || (idx>=m_neurons.size())) return NULL; return m_neurons[idx]; }
bool                SScHighwayNetwork::contains         (SSiHighwayNeuron *n) const     { return m_neurons.contains(n); }
bool                SScHighwayNetwork::connect          (int from, int to, double v)    { return SScHighwayNetwork::connect   (idx2n(from), idx2n(to), v ); }
bool                SScHighwayNetwork::disconnect       (int from, int to)              { return SScHighwayNetwork::disconnect(idx2n(from), idx2n(to)); }
void                SScHighwayNetwork::reset            ()                              { foreach(SSiHighwayNeuron* n, m_neurons) n->reset(); }
void                SScHighwayNetwork::trainingStep     (bool endOfCycle)
{
    foreach(SSiHighwayNeuron* n, m_neurons) n->trainingStep();
    if (endOfCycle) foreach(SSiHighwayNeuron* n, m_neurons) n->endOfCycle();
}
