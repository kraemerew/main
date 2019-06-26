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
    int addHighwayNeuron (const QString& name = QString());

    bool delNeuron(SSiHighwayNeuron* n);
    bool delNeuron(int idx);
    int n2idx(SSiHighwayNeuron* n) const;
    SSiHighwayNeuron* idx2n(int idx) const;
    void resetDedo() { foreach(SSiHighwayNeuron* n, m_neurons) n->reset(); }
    void connectForward();
    bool contains(SSiHighwayNeuron* n) const;
    bool connect(SSiHighwayNeuron* from, SSiHighwayNeuron* to, double v);
    bool disconnect(SSiHighwayNeuron* from, SSiHighwayNeuron* to);
    bool connect(int from, int to, double v);
    bool disconnect(int from, int to);

    SScVM tpVM() const;
    bool fromVM(const SScVM& vm);

    void trainingStep(bool endOfCycle)
    {
        resetDedo();
        foreach(SSiHighwayNeuron* n, m_neurons) n->trainingStep(endOfCycle);
    }

private:
    bool isFeedForward() const;
    int addNeuron(SSiHighwayNeuron::SSeNeuronType type, const QString& name = QString());

    QList<SSiHighwayNeuron*>  m_neurons;
};

#endif // HIGHWAYNETWORK_HPP
