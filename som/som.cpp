#include "som.hpp"
#include <Qt>
#include <QPair>
#include <QtMath>

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
        m_gav  =m_g;
        m_etaav=m_eta;
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
        update(diff(v,m_v),m_g,m_eta, 1.0);
    }
    void moveAway(const QVector<double>& v, double scale)
    {
        update(diff(m_v,v),m_gav,m_etaav,scale);
    }

private:
    void update(const QVector<double>& g, QVector<double>& grad, QVector<double>& eta, double scale)
    {
        Q_ASSERT(g.size()==m_v.size());
        for (int i=0; i<g.size(); ++i)
        {
            const double sgn = grad[i]*g[i];
            if (sgn>0) eta[i]*=1.2; else if (sgn<0) eta[i]*=0.5;
        }
        for (int i=0; i<g.size(); ++i)
        {
            if (g[i]>0) m_v[i]+=scale*eta[i]; else
            if (g[i]<0) m_v[i]-=scale*eta[i];
        }
        grad=g;
    }

    QVector<double> m_v, m_g, m_gav, m_eta, m_etaav;
    SScActivation* m_act;
};

class SScSortableSOMNeuron : public QPair<double,SScSOMNeuron*>
{
public:

    explicit SScSortableSOMNeuron(SScSOMNeuron* n, double net)
        : QPair<double,SScSOMNeuron*>(net,n) {}
    explicit SScSortableSOMNeuron(SScSOMNeuron* n, const QVector<double>& v)
        : QPair<double,SScSOMNeuron*>(n->net(v),n) {}

    bool operator <= (const SScSortableSOMNeuron& other) const { return (*this).first<=other.first; }

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
    foreach(SScSOMNeuron* n, m_el) if (Q_LIKELY(n!=sel)) n->moveAway(v,0.1); else n->moveTo(v);
}


void SScSOMNetwork::step(const QVector<double>& v, int nr)
{
    Q_ASSERT(nr>0);

    QList<SScSortableSOMNeuron> l;
    foreach(SScSOMNeuron* n, m_el) l << SScSortableSOMNeuron(n,v);


    std::sort(l.begin(),l.end());

    l.takeLast().second->moveTo(v);
    double min = l.first().first, max = l.first().first;
    foreach(const SScSortableSOMNeuron& sn, l) if (sn.first<min) min=sn.first; else if (sn.first>max) max = sn.first;

    if (min<max)
    {
        const double d = 1.0/(max-min);

        foreach(const SScSortableSOMNeuron& sn, l)
        {
            const double scale = 1.0-(d*(sn.first-min)),
                         rbf = exp(-scale*scale);
            sn.second->moveAway(v,rbf);
        }
    }
}

