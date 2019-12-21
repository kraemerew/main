#include "kernel.hpp"
#include "network.hpp"
#include "conv.hpp"
#include "ssctrainableparameter.hpp"
#include "convpatternprovider.hpp"
#include "imageprovider.hpp"
#include "convhelpers.hpp"

SScKernel::SScKernel(SScHighwayNetwork* network, const QSize& k, const QSize& str, const QSize& units, int depth)
    : m_network (network),
      m_a       (SScActivation::create(SScActivation::SWISH)),
      m_netset  (false),
      m_color   (depth==3),
      m_nrw     (k.width()*k.height()*depth),
      m_nrn     (units.width()*units.height()),
      m_ksz     (k),
      m_str     (str),
      m_units   (units)
{
    Q_CHECK_PTR(network);
    Q_ASSERT(m_nrn>0);
    Q_ASSERT(m_nrw>0);
    Q_ASSERT(depth==1 || depth==3);
    createNeurons();
    createWeights();
}

SScKernel::~SScKernel()
{
    delete m_a;
    clearNeurons();
    clearWeights();
}

SScConvNeuron* SScKernel::unit(int c, int r) const
{
    const int idx = (r*m_units.width())+c;
    return ((idx>=0) && (idx<m_neurons.size())) ? m_neurons[idx] : NULL;
}

void SScKernel::reset() { m_netset = false; foreach(auto n, m_neurons) n->reset(); }

bool SScKernel::activatePattern(const QVector<double> &pattern)
{
    if (pattern.size()!=m_w.size()*m_units.width()*m_units.height()) return false;
    QVector<double> w;
    w.reserve(m_w.size());
    for (int i=0; i<m_w.size(); ++i) w << m_w[i]->value();
    m_n.clear();
    m_n.reserve(m_neurons.size());
    m_n = SSnBlas::mxv(pattern,w);
    m_o.clear();
    m_o.reserve(m_n.size());
    foreach(auto v, m_n) m_o << m_a->activate(v);
    return true;
}

bool SScKernel::transform()
{
    QVector<double>  pattern;
    if (m_inputs.isEmpty())
    {
       pattern = m_network->ip()->get(m_ksz,m_str,m_units,m_color);
    }
    else
    {
        foreach(const auto& field, m_inputs)                   
            foreach(auto& n, field) pattern << n->out();
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
    for (int i=0; i<m_neurons.size(); ++i) if (m_inputs[i].contains(inneuron)) ret << m_neurons[i];
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

QVector<double> SScKernel::deltaw()
{
    Q_ASSERT(!m_neurons.isEmpty());
    QVector<double> ret; ret.fill(0.0,m_w.size());
   /* for (int n=0; n<m_neurons.size(); ++n)
    {
        SScConvNeuron* neuron = m_neurons[n];
        const QVector<double>& pat = m_currentpattern[n];
        for (int i=0; i<m_w.size(); ++i)
            ret[i] += -neuron->dedo()*pat[i]*neuron->act()->dev()*neuron->act()->gain();
    }
    const double scale = 1.0/(double)m_neurons.size();
    for (int i=0; i<ret.size(); ++i) ret[i]*=scale;
    */return ret;
}

void SScKernel::endOfCycle()
{
    foreach(auto tp, m_w)       tp->endOfCycle();
    foreach(auto n,  m_neurons) n ->endOfCycle();
}

void SScKernel::trainingStep()
{
    const auto v = deltaw();
    for (int i=0; i<m_w.size(); ++i) m_w[i]->update(v[i]);
    foreach(auto n, m_neurons) n->act()->update(n->dedo(),0.0);
}

QImage SScKernel::imOut() const { return SSnConvHelper::toImage(m_o,m_units); }
QImage SScKernel::imNet() const { return SSnConvHelper::toImage(m_n,m_units); }
