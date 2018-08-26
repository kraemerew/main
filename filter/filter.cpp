#include "filter.hpp"
#include "selector.hpp"
#include <QtMath>
/*
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
    SScSelector<uchar> sel;
    foreach(auto p, m_pixels_normalized) sel << get(p,x,y);
    return sel.get(m_selector);
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
*/

SScTSFilterMorphology::SScTSFilterMorphology(double sel) : m_selector(qBound(0.0,sel,1.0)), m_w(0), m_h(0), m_buflen(0) {}

bool SScTSFilterMorphology::setSelection(double sel)
{
    if ((sel>=0) && (sel<=1))
    {
        m_selector = sel;
        return true;
    }
    return false;
}
bool SScTSFilterMorphology::addPos(const SScPosProd& pp, int t)
{
    if (pp.isEmpty()) return false;
    m_pixels.insert(t,pp);
    return true;
}
bool SScTSFilterMorphology::addPos(const SScPos& pos, int t)
{
    return addPos(SScPosProd(pos),t);
}

void SScTSFilterMorphology::normalize()
{
    if (m_pixels_normalized.size()!=m_pixels.size())
    {
        m_pixels_normalized.clear();
        const int t0 = tmin(), t1 = tmax();
        m_buflen = t1-t0+1;
        qWarning(">>>>>>>>>>>>>>BUFLEN %d", (int)m_buflen);
        for(QMultiMap<int,SScPosProd>::iterator it = m_pixels.begin(); it!=m_pixels.end(); ++it)
            m_pixels_normalized.insert(it.key()-t1,it.value());
    }
}

uchar SScTSFilterMorphology::get(const SScPosProd& pp, int x, int y, const SScMatrix<uchar>& im) const
{
    int sh = -1;
    quint64 ret = 1;
    if (im.isEmpty()) return 0;
    foreach(const auto& p, pp)
    {
        const int xpos = im.xTor(x+p.x()), ypos = im.yTor(y+p.y());
        //qWarning("X %d+%d=%d Y %d+%d=%d", x, p.x(), xpos, y, p.y(),ypos);
        Q_ASSERT(xpos<(int)m_w);
        Q_ASSERT(ypos<(int)m_h);
        ret*=im.constLine(ypos)[xpos];
        ++sh;
    }

    if (sh>0)

        ret >>= (8*sh);
    return qBound((quint64)0,ret,(quint64)255);
}

SScMatrix<uchar> SScTSFilterMorphology::get()
{
    normalize();
    SScMatrix<uchar> ret(m_w,m_h);
    for (int x=0; x<m_w; ++x) for (int y=0; y<m_h; ++y)
    {
        const int v = get(x,y);
        ret.line(y)[x]=v;
    }
    return ret;
}

uchar SScTSFilterMorphology::get(int x, int y) const
{
    //qWarning("--------GET X %d Y %d BUFLEN %d CURRENR BUFLEN %d",x,y, m_buflen, m_inputs.size());
    //for(QMultiMap<int,SScPosProd>::const_iterator it = m_pixels_normalized.begin(); it!=m_pixels_normalized.end(); ++it)
   //   qWarning(">>>>>>>>TIME %d BUF %d", it.key(), m_inputs.size());
    if (m_inputs.size()>=m_buflen)
    {
        SScSelector<uchar> sel;
        for(QMultiMap<int,SScPosProd>::const_iterator it = m_pixels_normalized.begin(); it!=m_pixels_normalized.end(); ++it)
        {
            sel << get(it.value(),x,y,m_inputs[-it.key()]);
        }
        return qBound(0,qRound(sel.get(m_selector)),255);
    }
    return 0;
}

bool SScTSFilterMorphology::append(const SScMatrix<uchar> &src)
{
    if (src.isEmpty()) return false;
    if (m_inputs.isEmpty())
    {
        m_w = src.width();
        m_h = src.height();
    }
    if ((src.width()!=m_w) || (src.height()!=m_h)) return false;
    m_inputs.push_front(src);
    if (m_inputs.size()>m_buflen) m_inputs.pop_back();
    return m_inputs.size()>=m_buflen;
}
