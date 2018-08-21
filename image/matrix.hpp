
#ifndef SSCMATRIX_HPP
#define SSCMATRIX_HPP
#include <Qt>
#include <QVector>

template <typename T> class SScMatrix
{
public:
    virtual ~SScMatrix() {  }

    SScMatrix()
        : m_w(0), m_h(0)
    {
    }

    SScMatrix(quint32 w, quint32 h, T* data)
        : m_w(w), m_h(h)
    {
        Q_CHECK_PTR(data);
        Q_ASSERT(m_w*m_h>0);
        m_data.resize(w*h);
        memcpy(m_data.data(),data,w*h);
    }

    SScMatrix(quint32 w, quint32 h, T fillvalue = 0)
        : m_w(w), m_h(h)
    {
        Q_ASSERT(m_w*m_h>0);
        m_data.resize(w*h);
        fill(fillvalue);
    }
    inline void                     fill(T fillvalue){ m_data.fill(fillvalue,len()); }
    inline bool                     isEmpty () const { return (width()*height())==0; }
    inline quint32                  len     () const { return m_data.size(); }
    inline quint32                  width   () const { return m_w; }
    inline quint32                  height  () const { return m_h; }
    inline QPair<quint32,quint32>   size    () const { return QPair<quint32,quint32>(m_w,m_h); }

    const T* constLine(quint32 nr) const
    {
        Q_ASSERT(nr<m_h);
        return m_data.constData()+(m_w*nr);
    }

    T* line(quint32 nr)
    {
        Q_ASSERT(nr<m_h);
        return m_data.data()+(m_w*nr);
    }

private:
    quint32 m_w, m_h;
    QVector<T>  m_data;
};


#endif // SSCMATRIX_HPP
