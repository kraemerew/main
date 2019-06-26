#ifndef HIGHWAYNETWORK_HPP
#define HIGHWAYNETWORK_HPP
#include "sscneuron.hpp"
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
    int addHighwayNeuron (const QString& name = QString());

    bool delNeuron(SScNeuron* n);
    bool delNeuron(int idx);
    int n2idx(SScNeuron* n) const;
    SScNeuron* idx2n(int idx) const;
    void resetDedo() { foreach(SScNeuron* n, m_neurons) n->reset(); }
    void connectForward();
    bool contains(SScNeuron* n) const;
    bool connect(SScNeuron* from, SScNeuron* to, double v);
    bool disconnect(SScNeuron* from, SScNeuron* to);
    bool connect(int from, int to, double v);
    bool disconnect(int from, int to);

    SScVM tpVM() const;
    bool fromVM(const SScVM& vm);

    void trainingStep(bool endOfCycle)
    {
        resetDedo();
        foreach(SScNeuron* n, m_neurons) n->trainingStep(endOfCycle);
    }

private:
    bool isFeedForward() const;
    int addNeuron(SScNeuron::SSeNeuronType type, const QString& name = QString());

    QList<SScNeuron*>  m_neurons;
};

#endif // HIGHWAYNETWORK_HPP
