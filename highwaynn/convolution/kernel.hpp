#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <QVector>
#include <QList>
#include <QHash>
#include "../nnhelpers/sscactivation.hpp"

class SScImageProviderV2;
class SScTrainableParameter;
class SScHighwayNetwork;
class SScConvNeuron;

class SScKernel
{
public:
    /*!
     * \brief SScKernel
     * \param network
     * \param ksz
     * \param str
     * \param units
     * \param depth
     */
    explicit SScKernel(SScHighwayNetwork* network, const QSize& ksz, const QSize& str, const QSize& units, int depth = 1);
    /*!
     * \brief ~SScKernel
     */
    virtual ~SScKernel();
    /*!
     * \brief Return output activation image
     * \return
     */
    QImage imOut() const;
    /*!
     * \brief Return potential image
     * \return
     */
    QImage imNet() const;
    /*!
     * \brief Return unit a column and row
     * \param c
     * \param r
     * \return NULL for out of range cases
     */
    SScConvNeuron* unit(int c, int r) const;
    virtual void resetTraining()
    {
        m_iconcache.clear();
        m_fwdcache. clear();
        m_wpcache.  clear();
    }
    virtual void reset();
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

    QVector<double> deltaw();
    void trainingStep();
    void endOfCycle();
    virtual double net(quint32 idx)
    {
        if (!m_netset)
        {
            transform();
            m_netset = true;
        }

        Q_ASSERT(idx<(quint32)m_n.size());
        return m_n[idx];
    }

    SScConvNeuron* output(quint32 idx) const { return (idx<(quint32)m_neurons.size()) ? m_neurons[idx] : NULL; }
    void addInputField(const QVector<SScConvNeuron*>& v) { m_inputs << v; }
    QVariantMap toVM() const { return QVariantMap(); }
    bool fromVM(const QVariantMap&) { return false; }

protected:
    bool transform();
    bool activatePattern(const QVector<double> & pattern);
    void createNeurons();
    void createWeights();
    void clearNeurons();
    void clearWeights();

    SScHighwayNetwork*      m_network;
    SScActivation*          m_a;
    bool                    m_netset, m_color;
    int                     m_nrw, m_nrn;
    QSize                   m_ksz, m_str, m_units;


    QVector<SScTrainableParameter*>                                     m_w;
    QVector<double>                                                     m_n, m_o;
    QVector<SScConvNeuron*>                                             m_neurons;
    QVector<QVector<SScConvNeuron*> >                                   m_inputs;
    QHash<QPair<SScConvNeuron*,SScConvNeuron*>, SScTrainableParameter*> m_iconcache;
    QHash<SScConvNeuron*, QVector<SScConvNeuron*> >                     m_fwdcache;
    QHash<int,QVector<QPair<SScConvNeuron*,SScConvNeuron*> > >          m_wpcache;
};

#endif // KERNEL_HPP
