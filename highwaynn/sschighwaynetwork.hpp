#ifndef HIGHWAYNETWORK_HPP
#define HIGHWAYNETWORK_HPP
#include "sschighwayneuron.hpp"
#include "../nnhelpers/sscnetworkbase.hpp"

#include <QVariantMap>
#include <QMap>

class SScHighwayNetwork : public SScNetworkBase
{
public:
    SScHighwayNetwork();
    virtual ~SScHighwayNetwork();

    int addInputNeuron   (const QString& name = QString());
    int addHiddenNeuron  (const QString& name = QString());
    int addOutputNeuron  (const QString& name = QString());
    int addBiasNeuron    (const QString& name = QString());
    int addCarryNeuron   (const QString& name = QString());

    bool delNeuron(SSiHighwayNeuron* n);
    bool delNeuron(int idx);
    int n2idx(SSiHighwayNeuron* n) const;
    SSiHighwayNeuron* idx2n(int idx) const;
    void reset();
    void connectForward();
    bool contains(SSiHighwayNeuron* n) const;
    bool connect(SSiHighwayNeuron* from, SSiHighwayNeuron* to, double v);    
    bool disconnect(SSiHighwayNeuron* from, SSiHighwayNeuron* to);
    bool connect(int from, int to, double v);
    bool connect(int from, int to);

    /*!
     * \brief Make a highway connection
     * \param neuron    Highway neuron (must be hidden or output)
     * \param highway   Highway source (must be input or hiddenII
     * \param carry     Carry signal generator (must be hidden or output, should activate in [0;1])
     * \return
     */
    bool setHighway(int neuron, int highway, int carry);
    bool delHighway(int neuron);
    bool disconnect(int from, int to);

    QVariantMap toVM() const;
    bool fromVM(const QVariantMap& vm);

    void trainingStep(bool endOfCycle);

    bool setInput (int idx, double d);
    bool setTarget(int idx, double d);    

private:
    bool isFeedForward() const;
    int addNeuron(SSiHighwayNeuron::SSeNeuronType type, const QString& name = QString());
    inline int nextFreeIdx() const { for (int i=0; i<INT_MAX; ++i) if (!m_neurons.contains(i)) return i; Q_ASSERT(false); return -1; }
    QMap<int,SSiHighwayNeuron*>  m_neurons;
};

#endif // HIGHWAYNETWORK_HPP
