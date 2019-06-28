#ifndef HIGHWAYNETWORK_HPP
#define HIGHWAYNETWORK_HPP
#include "sschighwayneuron.hpp"
#include "../nnhelpers/sscvm.hpp"

#include <QMap>
class SScHighwayNetwork
{
public:
    SScHighwayNetwork();
    virtual ~SScHighwayNetwork();

    int addInputNeuron   (const QString& name = QString());
    int addHiddenNeuron  (const QString& name = QString());
    int addOutputNeuron  (const QString& name = QString());
    int addBiasNeuron    (const QString& name = QString());

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
    /*!
     * \brief Make a highway connection
     * \param neuron    Highway neuron (must be hidden or output)
     * \param highway   Highway source (must be input or hiddenII
     * \param carry     Carry signal generator (must be hidden or output, should activate in [0;1]
     * \return
     */
    bool setHighway(int neuron, int highway, int carry);
    bool delHighway(int neuron);
    bool disconnect(int from, int to);

    SScVM tpVM() const;
    bool fromVM(const SScVM& vm);

    void trainingStep(bool endOfCycle);

private:
    bool isFeedForward() const;
    int addNeuron(SSiHighwayNeuron::SSeNeuronType type, const QString& name = QString());

    QList<SSiHighwayNeuron*>  m_neurons;
};

#endif // HIGHWAYNETWORK_HPP
