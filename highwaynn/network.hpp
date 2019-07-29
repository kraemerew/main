#ifndef HIGHWAYNETWORK_HPP
#define HIGHWAYNETWORK_HPP
#include "neuron.hpp"
#include "../nnhelpers/sscnetworkbase.hpp"

#include <QVariantMap>
#include <QMap>

class SScHighwayNetwork : public SScNetworkBase
{
public:
    SScHighwayNetwork();
    virtual ~SScHighwayNetwork();

    inline QString name() const { return m_name; }
    inline void setName(const QString& s) { m_name=s; }

    QByteArray toData() const;
    bool fromData(const QByteArray&);
    bool load(const QString& filename);
    bool save(const QString& filename, bool compressed = false);
    inline int size() const { return m_neurons.size(); }

    inline void clear() { foreach(SSiHighwayNeuron* n, m_neurons) delete n; m_neurons.clear(); }
    int addInputNeuron   (const QString& name = QString());
    int addHiddenNeuron  (const QString& name = QString());
    int addOutputNeuron  (const QString& name = QString());
    int addBiasNeuron    (const QString& name = QString());
    int addCarryNeuron   (const QString& name = QString());
    int addMinPoolNeuron (const QString& name = QString());
    int addMedPoolNeuron (const QString& name = QString());
    int addMaxPoolNeuron (const QString& name = QString());

    bool delNeuron(SSiHighwayNeuron* n);
    bool delNeuron(int idx);
    int n2idx(const SSiHighwayNeuron* n) const;
    SSiHighwayNeuron* idx2n(int idx) const;
    void reset();
    bool contains(SSiHighwayNeuron* n) const;
    bool disconnect(SSiHighwayNeuron* from, SSiHighwayNeuron* to);


    bool connect(const QList<SSiHighwayNeuron*>& from, SSiHighwayNeuron* to, double v);
    bool connect(const QList<SSiHighwayNeuron*>& from, SSiHighwayNeuron* to);

    bool connect(SSiHighwayNeuron* from, SSiHighwayNeuron* to, double v);
    bool connect(int from, int to, const QVariantMap&);
    bool connect(int from, int to, double v);
    bool connect(int from, int to);
    bool connect(const QList<int>& from, int to, double v);
    bool connect(const QList<int>& from, int to);

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

    void dump();
    bool lockTraining(int nr, bool l)
    {
       if (!m_neurons.contains(nr)) return false;
       if (l)
       {
           if (m_locked.contains(nr)) return false;
           m_locked << nr;
           return true;
       }
       else
       {
           return m_locked.remove(nr);
       }
    }


private:
    bool isFeedForward() const;
    int addNeuron(SSiHighwayNeuron::Type type, const QString& name = QString());
    inline int nextFreeIdx() const { for (int i=0; i<INT_MAX; ++i) if (!m_neurons.contains(i)) return i; Q_ASSERT(false); return -1; }

    QString                     m_name;
    QMap<int,SSiHighwayNeuron*> m_neurons;
    QSet<int>                   m_locked;
};

#endif // HIGHWAYNETWORK_HPP
