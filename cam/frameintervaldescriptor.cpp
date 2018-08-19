#include "frameintervaldescriptor.hpp"
#include <QMap>
#include <QtMath>

class SScFIF : public QPair<quint32,quint32>
{
public:
    SScFIF() : QPair<quint32,quint32>(0,1) {}
    SScFIF(quint32 n, quint32 d) : QPair<quint32,quint32>(n,d) { Q_ASSERT(d!=0); }

    bool operator == (const SScFIF& other) const { return first*other.second==second*other.first; }
    bool operator <  (const SScFIF& other) const { return value() < other.value(); }
    bool operator >  (const SScFIF& other) const { return value() > other.value(); }
    bool operator <= (const SScFIF& other) const { return ((*this)<other) || ((*this)==other); }
    bool operator >= (const SScFIF& other) const { return ((*this)>other) || ((*this)==other); }
    double value     ()                    const { return (double)first/(double)second; }


    SScFIF operator +(const SScFIF& other) const
    {
        //qWarning("ADDING %u %u to %u %u",other.first,other.second,first,second);
        const quint32 n = (first*other.second)+(other.first*second), d = second*other.second;
        qWarning("Result %u %u -> %u %u",n,d,first,second);
        return SScFIF(n,d);
    }
};



SScFrameIntervalDescriptor::SScFrameIntervalDescriptor(quint32 n, quint32 d)
: m_nmin(n), m_dmin(d), m_nmax(n), m_dmax(d), m_nstep(0), m_dstep(1)
{
}

SScFrameIntervalDescriptor::SScFrameIntervalDescriptor(quint32 n_min, quint32 d_min, quint32 n_max, quint32 d_max, quint32 n_step, quint32 d_step)
    : m_nmin(n_min), m_dmin(d_min), m_nmax(n_max), m_dmax(d_max), m_nstep(n_step), m_dstep(d_step)
{}

void SScFrameIntervalDescriptor::dump() const
{
    if (discrete()) qWarning("Discrete frame interval %u/%u", m_nmin,m_dmin);
    else            qWarning("Stepwise frame interval %u/%u - %u/%u step %u/%u"
                             "", m_nmin,m_dmin,m_nmax,m_dmax,m_nstep,m_dstep);
}

bool SScFrameIntervalDescriptor::allowed(quint32 n, quint32 d) const
{
    SScFIF fmin   (m_nmin,m_dmin),
           fmax   (m_nmax,m_dmax),
           fstep  (m_nstep,m_dstep),
           frq    (n,d),
           fval = fmin;
    if (discrete()) return fmin==frq;
    else do
    {
        if (fval==frq) return true;
        fval=fval+fstep;
        if (fval>fmax) return false;
    }
    while (fval<=fmax);
    return false;
}


QPair<quint32,quint32> SScFrameIntervalDescriptor::fit(quint32 n, quint32 d) const
{
    if (discrete()) return QPair<quint32,quint32>(m_nmin,m_dmin);
    QMap<double,SScFIF> dmap;
    SScFIF min (m_nmin,m_dmin), max (m_nmax,m_dmax), step (m_nstep,m_dstep), val = min, rq(n,d);
    bool go_on = true;
    do
    {
        dmap[qAbs(val.value()-rq.value())]=val;
        val = val+step;
        if (val>max) go_on = false;
    }
    while (go_on);
    return dmap.first();
}

QPair<quint32,quint32> SScFrameIntervalDescriptor::fit(quint32 n, quint32 d, const QList<SScFrameIntervalDescriptor> &l)
{
    if (l.isEmpty() || (d==0)) return QPair<quint32,quint32>(0,0);
    SScFIF rq(n,d);
    QMap<double, QPair<quint32,quint32> > dmap;
    foreach(const SScFrameIntervalDescriptor& desc, l) if (desc.allowed(n,d)) return QPair<quint32,quint32>(n,d);
    foreach(const SScFrameIntervalDescriptor& desc, l)
    {
        auto p = desc.fit(n,d);
        dmap[qAbs(SScFIF(p.first,p.second).value()-rq.value())]=p;
    }
    return dmap.first();
}
