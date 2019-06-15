#include "filter.hpp"
#include "selector.hpp"
#include <QtMath>


SScTSFilterMorphology::SScTSFilterMorphology(double sel)
    : m_selector(NULL), m_w(0), m_h(0), m_buflen(0)
{
    setSelection(qBound(0.0,sel,1.0));
}

SScTSFilterMorphology::~SScTSFilterMorphology()
{
    if (m_selector) delete m_selector;
}
bool SScTSFilterMorphology::setSelection(double sel)
{
    if ((sel>=0) && (sel<=1))
    {
        if (m_selector) delete m_selector;
        if (sel==0) m_selector = new SScMinSelector<uchar>(); else
        if (sel==1) m_selector = new SScMaxSelector<uchar>(); else
                    m_selector = new SScSelector<uchar>(sel);
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

uchar SScTSFilterMorphology::get(const SScPosProd& pp, int x, int y, const SScUCMatrix& im) const
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

SScUCMatrix SScTSFilterMorphology::get()
{
    normalize();
    SScUCMatrix ret(m_w,m_h);
    for (int x=0; x<m_w; ++x) for (int y=0; y<m_h; ++y)
    {
        const int v = get(x,y);
        ret.line(y)[x]=v;
    }
    return ret;
}

uchar SScTSFilterMorphology::get(int x, int y) const
{
    if (m_inputs.size()>=m_buflen)
    {
        Q_CHECK_PTR(m_selector);
        m_selector->clear();
        for(QMultiMap<int,SScPosProd>::const_iterator it = m_pixels_normalized.begin(); it!=m_pixels_normalized.end(); ++it)
        {
            (*m_selector) << get(it.value(),x,y,m_inputs[-it.key()]);
        }
        return m_selector->get();
    }
    return 0;
}

bool SScTSFilterMorphology::append(const SScUCMatrix &src)
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
