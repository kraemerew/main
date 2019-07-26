#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <QVector>
#include <QList>

class SScTrainableParameter;
class SScHighwayNetwork;
class SScConvNeuron;

class SScKernel
{
public:
    explicit SScKernel(SScHighwayNetwork* network, int weights, int neurons);
    virtual ~SScKernel();
    inline double net(int idx) const { Q_ASSERT(idx<m_n.size()); return m_n[idx]; }

    bool activatePattern(const QList<QVector<double> >& pattern);
    SScConvNeuron* output(quint32 idx) const { return (idx<(quint32)m_neurons.size()) ? m_neurons[idx] : NULL; }
    QVariantMap toVM() const { return QVariantMap(); }
    bool fromVM(const QVariantMap&) { return false; }

protected:
    void createNeurons();
    void createWeights();
    void clearNeurons();
    void clearWeights();

    SScHighwayNetwork*              m_network;
    int                             m_nrw, m_nrn;
    QVector<SScTrainableParameter*> m_w;
    QVector<double>                 m_n;
    QVector<SScConvNeuron*>         m_neurons;
};

#endif // KERNEL_HPP
