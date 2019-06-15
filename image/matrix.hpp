
#ifndef SSCMATRIX_HPP
#define SSCMATRIX_HPP
#include <Qt>
#include <QVector>

template <typename T> class SScMatrix :  protected QVector<T>
{
public:
    virtual ~SScMatrix() {  }

    SScMatrix()
        : QVector<T>(), m_w(0), m_h(0)
    {}
    SScMatrix(const SScMatrix<T>& other)
        : QVector<T>(other), m_w(other.m_w), m_h(other.m_h)
    {}
    SScMatrix(quint32 w, quint32 h, T* dta)
        : QVector<T>(), m_w(w), m_h(h)
    {
        Q_CHECK_PTR(dta);
        Q_ASSERT(m_w*m_h>0);
        this->resize(w*h);
        memcpy(this->data(),dta,w*h);
    }
    SScMatrix(quint32 w, quint32 h)
        : QVector<T>(), m_w(w), m_h(h)
    {
        Q_ASSERT(m_w*m_h>0);
        this->resize(w*h);
    }
    SScMatrix(quint32 w, quint32 h, T fillvalue)
        : QVector<T>(), m_w(w), m_h(h)
    {
        Q_ASSERT(m_w*m_h>0);
        this->resize(w*h);
        fill(fillvalue);
    }
    inline void                     fill(T fillvalue){ QVector<T>::fill(fillvalue,this->len()); }
    inline bool                     isEmpty () const { return (width()*height())==0; }
    inline quint32                  len     () const { return QVector<T>::size(); }
    inline quint32                  width   () const { return m_w; }
    inline quint32                  height  () const { return m_h; }
    inline QPair<quint32,quint32>   size    () const { return QPair<quint32,quint32>(m_w,m_h); }
    inline void swap (QVector<T>& other) { this->swap(other); qSwap(this->m_w,other.m_w);qSwap(this->m_h,other.m_h); }
    inline void clear() { m_w=0; m_h=0; QVector<T>::clear(); }
    inline void operator =(const QVector<T>& other) { (*this)=other; m_w=other.m_w; m_h=other.m_h; }
    const T* constLine(quint32 nr) const
    {
        Q_ASSERT(nr<m_h);
        return this->constData()+(m_w*nr);
    }

    inline T* line(quint32 nr)
    {
        Q_ASSERT(nr<m_h);              
        return this->data()+(m_w*nr);
    }
    /*!
     * \brief Return y torus coordinate
     * \param y
     * \return
     */
    inline int yTor(int y) const
    {
        if (y<0) return -y; else
        if (y>=(int)m_h) return (2*(int)m_h) -y-1;
        else return y;
    }
    /*!
     * \brief Return x torus coordinate
     * \param x
     * \return
     */
    inline int xTor(int x) const
    {
        if (x<0) return -x; else
        if (x>=(int)m_w) return (2*(int)m_w)-x-1;
        else return x;
    }

protected:
    quint32 m_w, m_h;
};


class SScUCMatrix : public SScMatrix<uchar>
{
public:
    SScUCMatrix() : SScMatrix<uchar>() {}
    SScUCMatrix(const SScUCMatrix& other) : SScMatrix<uchar>(other) {}
    SScUCMatrix(quint32 w, quint32 h, uchar* dta) : SScMatrix<uchar>(w,h,dta) {}
    SScUCMatrix(quint32 w, quint32 h) : SScMatrix<uchar>(w,h) {}
    inline void negate() { for (quint32 i=0; i<(m_w*m_h); ++i)
            (*this)[i]=255-at(i); }
};


#endif // SSCMATRIX_HPP
