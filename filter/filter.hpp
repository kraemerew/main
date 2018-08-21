#ifndef FILTER_HPP
#define FILTER_HPP

#include <QVector>
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

#endif // FILTER_HPP
