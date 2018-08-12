#include "som.hpp"
#include <Qt>

QVector<double> diff(const QVector<double>& v1, const QVector<double>& v2)
{
    Q_ASSERT(v1.size()==v2.size());
    QVector<double> v;
    v.reserve(v1.size());
    for (int i=0; i<v1.size(); ++i) v.append(v1[i]-v2[i]);
    return v;
}

class SScSOMNeuron
{
public:

    SScSOMNeuron(int dim) : m_act(NULL)
    {
        setActivation(SScActivation::Act_Identity);
        init(dim);
    }
    SScSOMNeuron() : m_act(NULL) { setActivation(SScActivation::Act_Identity); }
    virtual ~SScSOMNeuron() { delete m_act; }

    inline void trainingReset(double etaamp = 0.1)
    {
        const int dim = m_v.size();
        m_g.    clear();
        m_eta.  clear();
        m_g.    reserve(dim);
        m_eta.  reserve(dim);
        for (int i=0; i<dim; ++i)
        {
            m_g.  append(0.0);
            m_eta.append(etaamp*(double)qrand()/(double)INT_MAX);
        }
    }
    inline bool isNull() const { return m_v.isEmpty(); }

    void init(int dim, double vamp = 1.0, double etaamp = 0.1)
    {
        Q_ASSERT(dim>0);
        m_v.clear();
        m_v.reserve(dim);
        for (int i=0; i<dim; ++i) m_v.  append(2*vamp*((double)qrand()/(double)INT_MAX)-vamp);
        trainingReset(etaamp);
    }

    inline void reset() { init(m_v.size()); }

    inline double act(const QVector<double>& v) const
    {
        return m_act->activate(net(v));
    }

    inline double net(const QVector<double>& v) const
    {
        Q_ASSERT(v.size()==m_v.size());
        double acc = 0;
        for (int i=0; i<v.size(); ++i) acc+=m_v[i]*v[i];
        return acc;
    }


    void setActivation(SScActivation::SSeActivation type)
    {
        Q_CHECK_PTR(m_act);
        if (m_act) delete m_act;
        m_act = SScActivation::create(type);
        Q_CHECK_PTR(m_act);
    }
    void moveTo(const QVector<double>& v)
    {
        update(diff(v,m_v));
    }
    void moveAway(const QVector<double>& v)
    {
        update(diff(m_v,v));
    }

private:
    void update(const QVector<double>& g)
    {
        Q_ASSERT(g.size()==m_v.size());
        for (int i=0; i<g.size(); ++i)
        {
            const double sgn = m_g[i]*g[i];
            if (sgn>0) m_eta[i]*=1.2; else if (sgn<0) m_eta[i]*=0.5;
        }
        for (int i=0; i<g.size(); ++i)
        {
            if (g[i]>0) m_v[i]+=m_eta[i]; else
            if (g[i]<0) m_v[i]-=m_eta[i];
        }
        m_g=g;
    }

    QVector<double> m_v, m_g, m_eta;
    SScActivation* m_act;
};


SScSOMNetwork::SScSOMNetwork(int elements, int dim)
{
    Q_ASSERT(elements>0);
    m_el.reserve(elements);
    for (int i=0; i<elements; ++i) m_el.append(new (std::nothrow) SScSOMNeuron(dim));
}

SScSOMNetwork::~SScSOMNetwork()
{
    foreach(SScSOMNeuron* n, m_el) delete n;
}


void SScSOMNetwork::setActivation(SScActivation::SSeActivation type)
{
    foreach(SScSOMNeuron* n, m_el) n->setActivation(type);
}

void SScSOMNetwork::step(const QVector<double>& v)
{
    SScSOMNeuron* sel = NULL;
    double max = 0;
    bool first = true;
    foreach(SScSOMNeuron* n, m_el)
    {
        const double net = n->net(v);
        if ((n->net(v)>max) || first)
        {
            first = false;
            max   = net;
            sel   = n;
        }
    }
    foreach(SScSOMNeuron* n, m_el) if (Q_LIKELY(n!=sel)) n->moveAway(v); else n->moveTo(v);
}
