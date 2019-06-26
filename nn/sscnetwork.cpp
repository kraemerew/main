#include "sscnetwork.hpp"
#include "../nnhelpers/ssccycledetector.hpp"
#include <QSet>

SScNetwork::SScNetwork()
{
}
SScNetwork::~SScNetwork()
{
    foreach(SScNeuron*n, m_neurons) delete n;
    m_neurons.clear();
}

int         SScNetwork::addNeuron   (SScNeuron::SSeNeuronType type, const QString& name) { m_neurons << SScNeuron::create(type, name); return m_neurons.size()-1; }
bool        SScNetwork::delNeuron   (SScNeuron* n)                  { return delNeuron(n2idx(n)); }
int         SScNetwork::n2idx       (SScNeuron* n) const            { return m_neurons.indexOf(n); }
SScNeuron*  SScNetwork::idx2n       (int idx) const                 { if ((idx<0) || (idx>=m_neurons.size())) return NULL; return m_neurons[idx]; }
bool        SScNetwork::contains    (SScNeuron *n) const            { return m_neurons.contains(n); }
bool        SScNetwork::connect     (int from, int to, double v)    { return SScNetwork::connect   (idx2n(from), idx2n(to), v ); }
bool        SScNetwork::disconnect  (int from, int to)              { return SScNetwork::disconnect(idx2n(from), idx2n(to)); }

bool SScNetwork::delNeuron   (int idx)
{
    SScNeuron* n = idx2n(idx);
    if (!n) return false;
    foreach(SScNeuron* other, m_neurons) (void) disconnect(n,other);
    m_neurons.removeAt(idx);
    delete n;
    return true;
}

bool SScNetwork::disconnect  (SScNeuron* from, SScNeuron* to)
{
    if (!contains(from) || !contains(to)) return false;
    return to->delInput(from);
}

void SScNetwork::connectForward()
{
    QMap<SScNeuron*,QSet<SScNeuron*> > m; //< outputs for each neuron
    foreach(SScNeuron* n, m_neurons) foreach(SScNeuron* inp, n->inputs())
    {
        m[inp] << n;    //< neuron inp feeds neuron n
    }
    foreach(SScNeuron* n, m_neurons) if (m.contains(n)) n->connectForward(m[n].toList());
}

bool SScNetwork::connect(SScNeuron* from, SScNeuron* to, double v)
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

bool SScNetwork::isFeedForward() const
{
    SScCycleDetector cdt(m_neurons.size());
    foreach(SScNeuron* to, m_neurons) foreach(SScNeuron* from, to->inputs()) cdt.addEdge(n2idx(from),n2idx(to));
    return !cdt.isCyclic();
}
