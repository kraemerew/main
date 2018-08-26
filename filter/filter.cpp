#include "filter.hpp"
#include <QtMath>

SScTSFilterMorphology::SScTSFilterMorphology(double sel) : m_selector(qBound(0.0,sel,1.0)) {}

bool SScTSFilterMorphology::append(const SScMatrix<uchar> &src)
{
    if (src.isEmpty()) return false;
    if (Q_UNLIKELY(m_pixels_normalized.size()!=m_pixels.size()))
    {
        m_pixels_normalized.clear();
        const int t0 = tmin(), t1 = tmax();
        m_buflen = (t1-t0)+1;
        Q_ASSERT(t1>=t0);
        foreach(auto p, m_pixels)
        {
            p.translate(0,0,-(t1-t0));
            m_pixels_normalized << p;
        }
    }
    m_inputs << src;
    if (m_inputs.size()>m_buflen) m_inputs.pop_front();
    return true;
}


bool SScTSFilterMorphology::addPos(const SScTSPosProd &pp)
{
    if (pp.isEmpty()) return false;
    m_pixels << pp;
    return true;
}
bool SScTSFilterMorphology::addPos(const SScTSPos& pos)
{
    SScTSPosProd pp; pp.append(pos);
   return addPos(pp);
}


uchar SScTSFilterMorphology::get(quint32 x, quint32 y) const
{
    QList<uchar> l;
    foreach(auto p, m_pixels_normalized)
    {
        l << get(p,x,y);
    }
    std::sort(l.begin(),l.end());
    const double sel = m_selector*(double)(l.size()-1);
    const int s0 = qFloor(sel), s1 = qCeil(sel);
    const double f1 = 1.0-(sel-(double)s0), f2 = s1-sel;
    return l[s0]*f1+l[s1]*f2;
}

uchar SScTSFilterMorphology::get(const SScTSPosProd& p, quint32 x, quint32 y) const
{
    quint64 ret = 1;
    int shf = -1;
    if (!p.isEmpty()) foreach(const SScTSPos& pos, p)
    {
        const int idx = m_inputs.size()-1+pos.t();
        if (idx<0)
        {
            ret*=0;
        }
        else if (idx>=m_inputs.size())
        {
            ret*=0;
        }
        else
        {
            const int xidx = m_inputs[idx].xTor(x+pos.x()),
                      yidx = m_inputs[idx].yTor(y+pos.y());
            ret *= m_inputs[idx].constLine(yidx)[xidx];
            ++shf;
        }
    }
    ret >>= (8*shf);
    return ret&0xff;
}
