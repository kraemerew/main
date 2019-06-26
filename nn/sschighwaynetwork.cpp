#include "sschighwaynetwork.hpp"
#include "sschighwayneuron.hpp"
#include "ssccycledetector.hpp"
#include <QSet>

SScHighwayNetwork::SScHighwayNetwork()
{
}
SScHighwayNetwork::~SScHighwayNetwork()
{
    foreach(SScNeuron*n, m_neurons) delete n;
    m_neurons.clear();
}

int SScHighwayNetwork::addNeuron   (SScNeuron::SSeNeuronType type, const QString& name)
{
    m_neurons << SScNeuron::create(type,name);
    return m_neurons.size()-1;
}

int         SScHighwayNetwork::addInputNeuron   (const QString& name)           { return addNeuron(SScNeuron::NeuronType_Input, name); }
int         SScHighwayNetwork::addHiddenNeuron  (const QString& name)           { return addNeuron(SScNeuron::NeuronType_Hidden,name); }
int         SScHighwayNetwork::addOutputNeuron  (const QString& name)           { return addNeuron(SScNeuron::NeuronType_Output,name); }
int         SScHighwayNetwork::addBiasNeuron    (const QString& name)           { return addNeuron(SScNeuron::NeuronType_Output,name); }
int         SScHighwayNetwork::addHighwayNeuron (const QString& name)           { return -1; }
bool        SScHighwayNetwork::delNeuron        (SScNeuron* n)                  { return delNeuron(n2idx(n)); }
int         SScHighwayNetwork::n2idx            (SScNeuron* n) const            { return m_neurons.indexOf(n); }
SScNeuron*  SScHighwayNetwork::idx2n            (int idx) const                 { if ((idx<0) || (idx>=m_neurons.size())) return NULL; return m_neurons[idx]; }
bool        SScHighwayNetwork::contains         (SScNeuron *n) const            { return m_neurons.contains(n); }
bool        SScHighwayNetwork::connect          (int from, int to, double v)    { return SScHighwayNetwork::connect   (idx2n(from), idx2n(to), v ); }
bool        SScHighwayNetwork::disconnect       (int from, int to)              { return SScHighwayNetwork::disconnect(idx2n(from), idx2n(to)); }

bool SScHighwayNetwork::delNeuron   (int idx)
{
    if (idx<0) return false;// <cant delete bias
    SScNeuron* n = idx2n(idx);
    if (!n) return false;
    foreach(SScNeuron* other, m_neurons) (void) disconnect(n,other);
    m_neurons.removeAt(idx);
    delete n;
    return true;
}

bool SScHighwayNetwork::disconnect  (SScNeuron* from, SScNeuron* to)
{
    if (!contains(from) || !contains(to)) return false;
    return to->delInput(from);
}

void SScHighwayNetwork::connectForward()
{
    QMap<SScNeuron*,QSet<SScNeuron*> > m; //< outputs for each neuron
    foreach(SScNeuron* n, m_neurons) foreach(SScNeuron* inp, n->inputs())
    {
        m[inp] << n;    //< neuron inp feeds neuron n
    }
    foreach(SScNeuron* n, m_neurons) if (m.contains(n)) n->connectForward(m[n].toList());
}

bool SScHighwayNetwork::connect(SScNeuron* from, SScNeuron* to, double v)
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
    foreach(SScNeuron* to, m_neurons) foreach(SScNeuron* from, to->inputs ()) cdt.addEdge(n2idx(from),n2idx(to));
    foreach(SScNeuron* to, m_neurons) foreach(SScNeuron* from, to->inputsC()) cdt.addEdge(n2idx(from),n2idx(to));
    return !cdt.isCyclic();
}
