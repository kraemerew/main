#ifndef FILTER_HPP
#define FILTER_HPP

#include "image/matrix.hpp"
#include <QVector>
#include <QMultiMap>

template <typename T> class SScSelectorBase;

class SScPos : public QPair<int,int>
{
public:
    explicit SScPos() : QPair<int,int>(0,0) {}
    explicit SScPos(int x, int y) : QPair<int,int>(x,y) {}
    inline int x() const { return first ; }
    inline int y() const { return second; }
};

class SScPosProd : public QList<SScPos>
{
public:
    SScPosProd(const SScPos& p) { (*this) << p; }
};

/*!
 * \brief Time-space coordinate
 */
struct SScTSPos
{
public:
    SScTSPos() : m_x(0), m_y(0), m_t(0) {}
    SScTSPos(int x, int y, int t) : m_x(x), m_y(y), m_t(t) {}
    inline int  x() const { return m_x; }
    inline int  y() const { return m_y; }
    inline int  t() const { return m_t; }
    inline void translate(int x, int y, int t) { m_x+=x; m_y+=y; m_t+=t; }
    /*!
     * \brief transfer xy to 1d position for 2d image processing
     * \param w image width
     * \return
     */
    inline int to1D(const int w) const { return m_x+(m_y*w); }

private:
    int m_x, m_y, m_t;
};

/*!
 * \brief Time-space product
 */
struct SScTSPosProd : public QVector<SScTSPos>
{
public:

    inline int xmin() const { int ret = !isEmpty() ? first().x() : 0; foreach(const SScTSPos& p, (*this)) if (p.x()<ret) ret = p.x(); return ret;  }
    inline int ymin() const { int ret = !isEmpty() ? first().y() : 0; foreach(const SScTSPos& p, (*this)) if (p.y()<ret) ret = p.y(); return ret;  }
    inline int tmin() const { int ret = !isEmpty() ? first().t() : 0; foreach(const SScTSPos& p, (*this)) if (p.t()<ret) ret = p.t(); return ret;  }
    inline int xmax() const { int ret = !isEmpty() ? first().x() : 0; foreach(const SScTSPos& p, (*this)) if (p.x()>ret) ret = p.x(); return ret;  }
    inline int ymax() const { int ret = !isEmpty() ? first().y() : 0; foreach(const SScTSPos& p, (*this)) if (p.y()>ret) ret = p.y(); return ret;  }
    inline int tmax() const { int ret = !isEmpty() ? first().t() : 0; foreach(const SScTSPos& p, (*this)) if (p.t()>ret) ret = p.t(); return ret;  }

    inline void translate(int x, int y, int t) { for (int i=0; i<size(); ++i) (*this)[i].translate(x,y,t); }
};

/*!
 * \brief A set of time space products
 */
class SScTSPosProdSet : public QVector<SScTSPosProd>
{
public:
    inline int xmin() const { int ret = !isEmpty() ? first().xmin() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.xmin()<ret) ret = p.xmin(); return ret;  }
    inline int ymin() const { int ret = !isEmpty() ? first().ymin() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.ymin()<ret) ret = p.ymin(); return ret;  }
    inline int tmin() const { int ret = !isEmpty() ? first().tmin() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.tmin()<ret) ret = p.tmin(); return ret;  }
    inline int xmax() const { int ret = !isEmpty() ? first().xmax() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.xmax()>ret) ret = p.xmax(); return ret;  }
    inline int ymax() const { int ret = !isEmpty() ? first().ymax() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.ymax()>ret) ret = p.ymax(); return ret;  }
    inline int tmax() const { int ret = !isEmpty() ? first().tmax() : 0; foreach(const SScTSPosProd& p, (*this)) if (p.tmax()>ret) ret = p.tmax(); return ret;  }

    inline void normalizeTime()
    {
        if (!isEmpty())
        {
            const int t = -tmax();
            for (int i=0; i<size(); ++i) (*this)[i].translate(0,0,t);
        }
    }
};

class SScTSFilterMorphology
{
public:
    explicit SScTSFilterMorphology(double sel = 0);
    virtual ~SScTSFilterMorphology();
    bool setSelection(double sel);
    bool append(const SScMatrix<uchar>& src);

    bool addPos(const SScPosProd& pp, int t = 0);
    bool addPos(const SScPos& pos, int t = 0);

    inline int tmin() const { int ret = !m_pixels.isEmpty() ? m_pixels.keys().first() : 0; foreach(auto t, m_pixels.keys()) if (t<ret) ret = t; return ret;  }
    inline int tmax() const { int ret = !m_pixels.isEmpty() ? m_pixels.keys().first() : 0; foreach(auto t, m_pixels.keys()) if (t>ret) ret = t; return ret;  }


    SScMatrix<uchar> get();

private:

    uchar get(const SScPosProd& pp, int x, int y, const SScMatrix<uchar>& im) const;
    uchar get(int x, int y) const;

    /*!
     * \brief Transfer time index so that we have max index 0
     */
    void normalize();

    SScSelectorBase<uchar>*     m_selector;
    quint32                     m_w, m_h, m_buflen;
    QList<SScMatrix<uchar> >    m_inputs;
    QMultiMap<int,SScPosProd>   m_pixels, m_pixels_normalized;

};
#endif // FILTER_HPP
