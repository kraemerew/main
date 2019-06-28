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
    foreach(SSiHighwayNeuron* n, m_neurons) foreach(SSiHighwayNeuron* inp, n->allInputs())
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
    foreach(SSiHighwayNeuron* to, m_neurons)
    {
        const int toidx = n2idx(to);
        foreach(SSiHighwayNeuron* from, to->allInputs ()) cdt.addEdge(n2idx(from),toidx);
    }
    return !cdt.isCyclic();
}

bool SScHighwayNetwork::setHighway(int neuron, int highway, int carry)
{
    SSiHighwayNeuron* n = idx2n(neuron), *hwn = idx2n(highway), *cn = idx2n(carry);
    if (n && hwn && cn && n->connectHighway(hwn,cn))
    {
        if (isFeedForward()) return true;
        delHighway(neuron);
    }
    return false;
}
bool SScHighwayNetwork::delHighway(int neuron)
{
     SSiHighwayNeuron* n = idx2n(neuron);
     if (n)
     {
         n->connectHighway(NULL,NULL);
         return true;
     }
     return false;
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
    //foreach(SSiHighwayNeuron* n, m_neurons)  qWarning("%s NET %lf OUT %lf", qPrintable(n->name()), n->net(),n->out());
    //foreach(SSiHighwayNeuron* n, m_neurons) qWarning("%s CARRY %lf HW %lf OUT %lf", qPrintable(n->name()), n->carry(), n->highway(), n->out());
QSet<QString> toTrain;
//toTrain <<"C" << "H1" << "H2" << "H3" << "H4" << "Out";
    foreach(SSiHighwayNeuron* n, m_neurons) if (toTrain.isEmpty() || toTrain.contains(n->name())) n->trainingStep();
    if (endOfCycle) foreach(SSiHighwayNeuron* n, m_neurons) if (toTrain.isEmpty() || toTrain.contains(n->name())) n->endOfCycle();
}
