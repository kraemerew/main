#include "sschighwaynetwork.hpp"
#include "sschighwayneuron.hpp"
#include "../nnhelpers/ssccycledetector.hpp"
#include "../nnhelpers/sscvm.hpp"
#include <QSet>

SScHighwayNetwork::SScHighwayNetwork() : SScNetworkBase()
{
}
SScHighwayNetwork::~SScHighwayNetwork()
{
    foreach(SSiHighwayNeuron* n, m_neurons) delete n;
    m_neurons.clear();
}

int SScHighwayNetwork::addNeuron   (SSiHighwayNeuron::Type type, const QString& name)
{
    SSiHighwayNeuron* n = SSiHighwayNeuron::create(this,type,name);
    Q_CHECK_PTR(n);
    if (n) switch(type)
    {
        case SSiHighwayNeuron::Bias:
        case SSiHighwayNeuron::Input:
        break;
        case SSiHighwayNeuron::Hidden:
            n->setActivation(hActType(), getRandomGainValue());
        break;
        case SSiHighwayNeuron::Carry:
            n->setActivation(cActType(),getRandomGainValue());
        break;
        case SSiHighwayNeuron::Output:
            n->setActivation(oActType(),getRandomGainValue());
        break;
        case SSiHighwayNeuron::Last: break;
    }
    if (n)
    {
        n->act()->setTrainingType(trainingType());
        const int ret = nextFreeIdx();
        m_neurons[ret] = n;
        return ret;
    }
    return -1;
}

bool SScHighwayNetwork::delNeuron   (int idx)
{
    SSiHighwayNeuron* n = idx2n(idx);
    if (!n) return false;
    foreach(SSiHighwayNeuron* other, m_neurons) (void) disconnect(n,other);
    m_neurons.remove(idx);
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
    to->addInput(from,v,trainingType());

    if (!isFeedForward())
    {
        qWarning("err");

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
    if (n && hwn && cn && cn->act()->canCarry() && n->connectHighway(hwn,cn))
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

bool                SScHighwayNetwork::setInput         (int idx, double v)             { SSiHighwayNeuron* n = idx2n(idx); return n ? n->setInput(v) : false; }
bool                SScHighwayNetwork::setTarget        (int idx, double v)             { SSiHighwayNeuron* n = idx2n(idx); return n ? n->setTarget(v) : false; }

int                 SScHighwayNetwork::addInputNeuron   (const QString& name)           { return addNeuron(SSiHighwayNeuron::Input,  name); }
int                 SScHighwayNetwork::addHiddenNeuron  (const QString& name)           { return addNeuron(SSiHighwayNeuron::Hidden, name); }
int                 SScHighwayNetwork::addOutputNeuron  (const QString& name)           { return addNeuron(SSiHighwayNeuron::Output, name); }
int                 SScHighwayNetwork::addBiasNeuron    (const QString& name)           { return addNeuron(SSiHighwayNeuron::Bias,   name); }
int                 SScHighwayNetwork::addCarryNeuron   (const QString& name)           { return addNeuron(SSiHighwayNeuron::Carry,  name); }

bool                SScHighwayNetwork::delNeuron        (SSiHighwayNeuron* n)           { return delNeuron(n2idx(n)); }
int                 SScHighwayNetwork::n2idx            (SSiHighwayNeuron* n) const     { foreach(int i, m_neurons.keys()) if (m_neurons[i]==n) return i; return -1; }
SSiHighwayNeuron*   SScHighwayNetwork::idx2n            (int idx) const                 { if ((idx<0) || !m_neurons.contains(idx)) return NULL; return m_neurons[idx]; }
bool                SScHighwayNetwork::contains         (SSiHighwayNeuron *n) const     { return m_neurons.values().contains(n); }
bool                SScHighwayNetwork::connect          (int from, int to, double v)    { return SScHighwayNetwork::connect   (idx2n(from), idx2n(to), v ); }
bool                SScHighwayNetwork::connect          (int from, int to)              { return connect(from,to,getRandomConnectionValue()); }
bool                SScHighwayNetwork::disconnect       (int from, int to)              { return SScHighwayNetwork::disconnect(idx2n(from), idx2n(to)); }
void                SScHighwayNetwork::reset            ()                              { foreach(SSiHighwayNeuron* n, m_neurons) n->reset(); }
void                SScHighwayNetwork::trainingStep     (bool endOfCycle)
{
    foreach(SSiHighwayNeuron* n, m_neurons.values()) n->trainingStep();
    if (endOfCycle) foreach(SSiHighwayNeuron* n, m_neurons.values()) n->endOfCycle();
}

bool SScHighwayNetwork::connect(int from, int to, const QVariantMap& vm)
{
    disconnect(from,to);
    SSiHighwayNeuron* n1 = idx2n(from), *n2 = idx2n(to);
    if (!n1 || !n2) return false;
    SScTrainableParameter* tp = SScTrainableParameter::create(vm);
    if (tp)
    {
        n2->addInput(n1,tp);

        if (!isFeedForward())
        {
            (void) disconnect(from,to);
            return false;
        }

        return true;
    }
    return false;
}

QVariantMap SScHighwayNetwork::toVM() const
{
    QVariantMap ret;
    ret["NET_PRESETS"] = SScNetworkBase::toVM();
    foreach(int key, m_neurons.keys()) ret[QString("NEURON_%1").arg(key)] = m_neurons[key]->toVM();
    return ret;
}

bool SScHighwayNetwork::fromVM(const QVariantMap& vm)
{
    QMap<int,QVariantMap> cache;
    foreach(const QString& key, vm.keys())
    {
        if (key=="NET_PRESETS") SScNetworkBase::fromVM(vm[key].toMap());
        else if (key.startsWith("NEURON"))
        {
            bool ok = false;
            const int idx = key.split("_").last().toInt(&ok);
            if (ok && (idx>=0))
            {
                SSiHighwayNeuron* n = SSiHighwayNeuron::create(this,vm[key].toMap());
                if (n)
                {
                    m_neurons[idx]=n;
                    cache[idx] = vm[key].toMap();
                }
            }
        }
    }

    foreach(SSiHighwayNeuron* n, m_neurons) n->fromVM(cache[n2idx(n)]);
    return true;
}
