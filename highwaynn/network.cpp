#include "network.hpp"
#include "neuron.hpp"
#include "../nnhelpers/ssccycledetector.hpp"
#include "../nnhelpers/sscvm.hpp"
#include "../nnhelpers/ssnjsonhandler.hpp"
#include <QSet>
#include <QFile>

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
        case SSiHighwayNeuron::Conv:
            n->setActivation(hActType(),getRandomGainValue());
        break;
        case SSiHighwayNeuron::MinPool: break;
        case SSiHighwayNeuron::MaxPool: break;
        case SSiHighwayNeuron::MedPool: break;
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
    m_locked.remove(idx);
    delete n;
    return true;
}

bool SScHighwayNetwork::disconnect  (SSiHighwayNeuron* from, SSiHighwayNeuron* to)
{
    if (!contains(from) || !contains(to)) return false;
    return to->delConnection(from);
}

bool SScHighwayNetwork::connect(const QList<int>& from, int to, double v)
{
    bool ret = true;
    foreach(auto fr, from) if (!connect(fr,to,v)) { ret = false; break; }
    if (!isFeedForward()) ret = false;
    if (!ret) foreach(auto fr, from) disconnect(fr,to);
    return ret;
}
bool SScHighwayNetwork::connect(const QList<int>& from, int to)
{
    bool ret = true;
    foreach(auto fr, from) if (!connect(fr,to,getRandomConnectionValue())) { ret = false; break; }
    if (!isFeedForward()) ret = false;
    if (!ret) foreach(auto fr, from) disconnect(fr,to);
    return ret;
}

bool SScHighwayNetwork::connect(const QList<SSiHighwayNeuron*>& from, SSiHighwayNeuron* to, double v)
{
    bool ret = true;
    foreach(auto fr, from) if (!connect(fr,to,v)) { ret = false; break; }
    if (!isFeedForward()) ret = false;
    if (!ret) foreach(auto fr, from) disconnect(fr,to);
    return ret;
}

bool SScHighwayNetwork::connect(const QList<SSiHighwayNeuron*>& from, SSiHighwayNeuron* to)
{
    bool ret = true;
    foreach(auto fr, from) if (!connect(fr,to,getRandomConnectionValue())) { ret = false; break; }
    if (!isFeedForward()) ret = false;
    if (!ret) foreach(auto fr, from) disconnect(fr,to);
    return ret;
}

bool SScHighwayNetwork::connect(SSiHighwayNeuron* from, SSiHighwayNeuron* to, double v)
{   
    if (!contains(from) || !contains(to)) return false;
    to->addConnection(from,v,trainingType());

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
int                 SScHighwayNetwork::addMinPoolNeuron (const QString& name)           { return addNeuron(SSiHighwayNeuron::MinPool,name); }
int                 SScHighwayNetwork::addMaxPoolNeuron (const QString& name)           { return addNeuron(SSiHighwayNeuron::MaxPool,name); }
int                 SScHighwayNetwork::addMedPoolNeuron (const QString& name)           { return addNeuron(SSiHighwayNeuron::MedPool,name); }

bool                SScHighwayNetwork::delNeuron        (SSiHighwayNeuron* n)           { return delNeuron(n2idx(n)); }
int                 SScHighwayNetwork::n2idx            (const SSiHighwayNeuron* n)const{ foreach(int i, m_neurons.keys()) if (m_neurons[i]==n) return i; return -1; }
SSiHighwayNeuron*   SScHighwayNetwork::idx2n            (int idx) const                 { if ((idx<0) || !m_neurons.contains(idx)) return NULL; return m_neurons[idx]; }
bool                SScHighwayNetwork::contains         (SSiHighwayNeuron *n) const     { return m_neurons.values().contains(n); }
bool                SScHighwayNetwork::connect          (int from, int to, double v)    { return SScHighwayNetwork::connect   (idx2n(from), idx2n(to), v ); }
bool                SScHighwayNetwork::connect          (int from, int to)              { return connect(from,to,getRandomConnectionValue()); }
bool                SScHighwayNetwork::disconnect       (int from, int to)              { return SScHighwayNetwork::disconnect(idx2n(from), idx2n(to)); }
void                SScHighwayNetwork::reset            ()                              { foreach(SSiHighwayNeuron* n, m_neurons) n->reset(); }
void                SScHighwayNetwork::trainingStep     (bool endOfCycle)
{
    for (auto it= m_neurons.begin(); it!=m_neurons.end(); ++it) if (!m_locked.contains(it.key())) it.value()->trainingStep();
    if (endOfCycle)
        for (auto it= m_neurons.begin(); it!=m_neurons.end(); ++it) if (!m_locked.contains(it.key())) it.value()->endOfCycle();
}

bool SScHighwayNetwork::connect(int from, int to, const QVariantMap& vm)
{
    disconnect(from,to);
    SSiHighwayNeuron* n1 = idx2n(from), *n2 = idx2n(to);
    if (!n1 || !n2) return false;
    SScTrainableParameter* tp = SScTrainableParameter::create(vm);
    if (tp)
    {
        n2->addConnection(n1,tp);

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
    if (!name().isEmpty()) ret["NAME"] = name();
    foreach(int key, m_neurons.keys()) ret[QString("NEURON_%1").arg(key)] = m_neurons[key]->toVM();
    return ret;
}

bool SScHighwayNetwork::fromVM(const QVariantMap& vm)
{
    if (vm.isEmpty()) return false;
    clear();
    QMap<int,QVariantMap> cache;
    SScVM sscvm(vm);
    SScNetworkBase::fromVM(sscvm.vmToken("NET_PRESETS"));
    setName(sscvm.stringToken("NAME"));
    foreach(const QString& key, sscvm.keys()) if (key.startsWith("NEURON"))
    {
        bool ok = false;
        const int idx = key.split("_").last().toInt(&ok);
        if (ok && (idx>=0))
        {
            cache[idx] = sscvm.vmToken(key);
            SSiHighwayNeuron* n = SSiHighwayNeuron::create(this,cache[idx]);
            if (n)
            {
                m_neurons[idx]=n;
            }
        }
    }
    // Complete the initialization as well as the connection
    foreach(int idx, m_neurons.keys())
    {
        m_neurons[idx]->fromVM(cache[idx]);
    }

    return true;
}

QByteArray  SScHighwayNetwork::toData() const
{
    return SSnJsonHandler::toData(toVM());
}

bool SScHighwayNetwork::fromData(const QByteArray& data)
{
    bool ok = false;
    const QVariantMap vm = SSnJsonHandler::fromData(data,ok);
    return (!ok || vm.isEmpty()) ? false : fromVM(vm);
}

bool SScHighwayNetwork::save(const QString &filename, bool compressed)
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly))
    {
        QByteArray ba = toData();
        if (compressed) ba = qCompress(ba);
        f.write(ba);
        return true;
    }
    return false;
}

bool SScHighwayNetwork::load(const QString& filename)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {
        const QByteArray ba = f.readAll(), uc = qUncompress(ba);
        return uc.isEmpty() ? fromData(ba) : fromData(uc);
    }
    return false;
}

void SScHighwayNetwork::dump()
{
    qWarning("Highway network: %d neurons", size());
    foreach(SSiHighwayNeuron* n, m_neurons) n->dump();
}
