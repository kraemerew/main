#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <QVector>
#include <QList>
#include <QHash>

class SScTrainableParameter;
class SScHighwayNetwork;
class SScConvNeuron;

class SScKernel
{
public:
    explicit SScKernel(SScHighwayNetwork* network, int weights, int neurons);
    virtual ~SScKernel();
    virtual void reset() { m_netset = false; m_iconcache.clear(); m_fwdcache.clear(); m_wpcache.clear(); }
    /*!
     * \brief Delivers all neurons pairs connected by weight #idx
     * \param idx
     * \return
     */
    QVector<QPair<SScConvNeuron*,SScConvNeuron*> > sharedConnectionPairs(int idx);
    /*!
     * \brief find all forward going connection from a neuron in the input layer to the output layer
     * \param inneuron
     * \return
     */
    QVector<SScConvNeuron*> fwdConnections(SScConvNeuron* inneuron);
    /*!
     * \brief Return which weight connects input layer neuron to output layer neuron
     * \param inneuron
     * \param outneuron
     * \return
     */
    SScTrainableParameter* icon(SScConvNeuron* inneuron, SScConvNeuron* outneuron);
    double iconValue(SScConvNeuron* inneuron, SScConvNeuron* outneuron);
    inline double net(quint32 idx)
    {
        if (!m_netset && !m_inputs.isEmpty())
        {
            transform();
            m_netset = true;
        }
        Q_ASSERT(idx<m_n.size());
        return m_n[idx];
    }

    bool activatePattern(const QVector<QVector<double> >& pattern);
    bool transform();
    SScConvNeuron* output(quint32 idx) const { return (idx<(quint32)m_neurons.size()) ? m_neurons[idx] : NULL; }
    void addInputField(const QVector<SScConvNeuron*>& v) { m_inputs << v; }
    QVariantMap toVM() const { return QVariantMap(); }
    bool fromVM(const QVariantMap&) { return false; }

protected:
    void createNeurons();
    void createWeights();
    void clearNeurons();
    void clearWeights();

    SScHighwayNetwork*                  m_network;
    bool                                m_netset;
    int                                 m_nrw, m_nrn;
    QVector<SScTrainableParameter*>     m_w;
    QVector<double>                     m_n;
    QVector<SScConvNeuron*>             m_neurons;
    QVector<QVector<SScConvNeuron*> >   m_inputs;
    QHash<QPair<SScConvNeuron*,SScConvNeuron*>, SScTrainableParameter*> m_iconcache;
    QHash<SScConvNeuron*, QVector<SScConvNeuron*> > m_fwdcache;
    QHash<int,QVector<QPair<SScConvNeuron*,SScConvNeuron*> > > m_wpcache;
};

#endif // KERNEL_HPP
