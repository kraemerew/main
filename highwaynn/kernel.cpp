#include "kernel.hpp"
#include "network.hpp"
#include "conv.hpp"
#include "ssctrainableparameter.hpp"
#include "convpatternprovider.hpp"

SScKernel::SScKernel(SScHighwayNetwork* network, SScConvPatternProvider* pp, int weights, int neurons)
    : m_network (network),
      m_pp      (pp),
      m_netset  (false),
      m_nrw     (weights),
      m_nrn     (neurons)
{
    Q_CHECK_PTR(network);
    Q_ASSERT(m_nrn>0);
    Q_ASSERT(m_nrw>0);
    createNeurons();
    createWeights();
}

SScKernel::~SScKernel()
{
    clearNeurons();
    clearWeights();
}

void SScKernel::reset() { m_netset = false; foreach(auto n, m_neurons) n->reset(); }

bool SScKernel::activatePattern(const QVector<QVector<double> > &pattern)
{
    QVector<double> w;
    w.reserve(m_w.size());
    for (int i=0; i<m_w.size(); ++i) w << m_w[i]->value();
    m_n.clear();
    m_n.reserve(m_neurons.size());
    bool ret = true;
    foreach(const QVector<double>& dv, pattern)
        if (dv.size()==w.size()) m_n << SSnBlas::dot(w,dv);
        else                  {  m_n << 0.0; ret = false; }
    return ret;
}

bool SScKernel::transform()
{
    QVector<QVector<double> > pattern;
    if (m_pp)
    {
        pattern = m_pp->currentPattern();
    }
    else
    {
        pattern.reserve(m_inputs.size());
        foreach(const auto& field, m_inputs)
        {
            QVector<double> v;
            v.reserve(field.size());
            foreach(auto& n, field) v << n->out();
            pattern << v;
        }
    }

    return activatePattern(pattern);
}

void SScKernel::clearNeurons()
{
    foreach(SScConvNeuron* n,m_neurons) delete n;
    m_neurons.clear();
}
void SScKernel::clearWeights()
{
    foreach(SScTrainableParameter* tp,m_w) delete tp;
    m_w.clear();
}
void SScKernel::createNeurons()
{
    while (m_neurons.size()<m_nrn)
    {
        SScConvNeuron* cn = new (std::nothrow) SScConvNeuron(m_network);
        cn->setKernel(this,m_neurons.size());// tell neuron the kernel and its own index in the neuron sequence
        m_neurons << cn;
    }
}
void SScKernel::createWeights()
{
    while (m_w.size()<m_nrw)
    {
        m_w << SScTrainableParameter::create(SScTrainableParameter::ADAM, (double)qrand()/(double)RAND_MAX); //<< TODO - reset
    }
}

QVector<SScConvNeuron*> SScKernel::fwdConnections(SScConvNeuron* inneuron)
{
    auto it = m_fwdcache.find(inneuron);
    if (it!=m_fwdcache.end()) return it.value();

    QSet<SScConvNeuron*> ret;
    for (int i=0; i<m_inputs.size(); ++i) if (m_inputs[i].contains(inneuron) && (i<m_neurons.size())) ret << m_neurons[i];
    m_fwdcache[inneuron]=ret.toList().toVector();
    return m_fwdcache[inneuron];
}

SScTrainableParameter* SScKernel::icon(SScConvNeuron* inneuron, SScConvNeuron* outneuron)
{
    QPair<SScConvNeuron*,SScConvNeuron*> p(inneuron,outneuron);
    auto it = m_iconcache.find(p);
    if (it!=m_iconcache.end()) return it.value();
    for (int i = 0; i<m_neurons.size(); ++i) if (m_neurons[i]==outneuron)
    {
        for (int j=0; j<m_inputs[i].size(); ++j) if (m_inputs[i][j]==inneuron)
        {
            m_iconcache[p]=m_w[j];
            return m_w[j];
        }
    }
    return m_iconcache[p]=NULL;
}
double SScKernel::iconValue(SScConvNeuron* inneuron, SScConvNeuron* outneuron)
{
    auto tp = icon(inneuron,outneuron);
    return tp ? tp->value() : 0.0;
}

QVector<QPair<SScConvNeuron*,SScConvNeuron*> > SScKernel::sharedConnectionPairs(int idx)
{
    QVector<QPair<SScConvNeuron*,SScConvNeuron*> > ret;
    auto it = m_wpcache.find(idx);
    if (it!=m_wpcache.end()) return it.value();

    for (int i=0; i<m_neurons.size(); ++i) if (idx<m_inputs[i].size())
    {
        auto on = m_neurons[i];
        auto in = m_inputs[i][idx];
        ret << QPair<SScConvNeuron*, SScConvNeuron*>(in,on);
    }
    m_wpcache[idx]=ret;
    return ret;
}
