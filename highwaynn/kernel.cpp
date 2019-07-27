#include "kernel.hpp"
#include "network.hpp"
#include "conv.hpp"

SScKernel::SScKernel(SScHighwayNetwork* network, int weights, int neurons)
    : m_network (network),
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
    pattern.reserve(m_inputs.size());
    foreach(const auto& field, m_inputs)
    {
        QVector<double> v; v.reserve(field.size());
        foreach(auto& n, field) v << n->out();
        pattern << v;
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
        cn->setKernel(this,m_neurons.size());
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

