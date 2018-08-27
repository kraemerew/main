#ifndef FRAMEREADER_HPP
#define FRAMEREADER_HPP
#include <QImage>


class SScFrameReaderBase
{
public:
    SScFrameReaderBase(quint32 w, quint32 h) : m_w(w), m_h(h)
    {
        Q_ASSERT(m_w>0);
        Q_ASSERT(m_h>0);
    }
    virtual ~SScFrameReaderBase() {}
    static SScFrameReaderBase* create(const QString& fourcc, quint32 w, quint32 h);
    virtual QImage get(void*,quint32) const =0 ;
protected:
    quint32 m_w, m_h;
};

#endif // FRAMEREADER_HPP
