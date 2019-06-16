#ifndef MOMENTS_HPP
#define MOMENTS_HPP

#include "matrix.hpp"
#include <QPair>
#include <QMap>
#include <QtMath>

template <typename T> class SScMoments
{
public:
    SScMoments() {}
    SScMoments(const SScMatrix<T> &m) : m_mtx(m)
    {
    }

    SScMoments(const QVector<QPair<int,int> >& pos, const QVector<T>& v) : m_v(v)
    {
        m_pos = normalized(pos);
        if (!v.isEmpty())
        {
            Q_ASSERT(pos.size()==v.size());
        }
    }

    /*!
     * \brief Raw moment
     * \param i
     * \param j1
     * \return
     */
    long double getM(int i, int j)
    {
        QPair<int,int> c(i,j);
        if (!moment_m.contains(c))
        {
            long double ret = 0;
            if (!m_mtx.isEmpty())
            {
                QVector<long double> xp, yp;
                for (quint32 k=0; k<m_mtx.width(); ++k)  xp << qPow((long double)k/(long double)(m_mtx.width ()-1),i);
                for (quint32 l=0; l<m_mtx.height(); ++l) yp << qPow((long double)l/(long double)(m_mtx.height()-1),j);

                for (quint32 l=0; l<m_mtx.height(); ++l)
                {
                    long double lsum = 0;
                    T* line = m_mtx.line(l);
                    for (quint32 k=0; k<m_mtx.width(); ++k) if (line[k]!=0.0) lsum += (xp[k]*(long double)line[k]);
                    lsum*=yp[l];
                    ret+=lsum;
                }
            }
            else
            {
                QPair<double,double> p;

                int k = -1;
                if (m_v.isEmpty()) foreach(p, m_pos)
                {
                    const long double x = p.first, y = p.second;
                    ret = ret + (long double)(qPow(x,i)*qPow(y,j));
                }
                else foreach(p, m_pos)
                {
                    ++k;
                    const long double x = p.first, y = p.second;
                    ret = ret + (long double)m_v[k] * (long double)(qPow(x,i)*qPow(y,j));
                }
            }            
            moment_m[c]=ret;
        }
        return moment_m[c];
    }

    /*!
     * \brief Central moment
     * \param i
     * \param j
     * \return
     */
    long double getMu(int i, int j)
    {
        QPair<int,int> c(i,j);
        if (!moment_mu.contains(c))
        {
            if ((i==0) && (j==0)) return getM(0,0);

            long double xm = xMean(), ym = yMean(), ret = 0;
            if (!m_mtx.isEmpty())
            {
                QVector<long double> xp, yp;
                for (quint32 k=0; k<m_mtx.width (); ++k) xp << qPow(((long double)k/(long double)(m_mtx.width ()-1))-xm,i);
                for (quint32 l=0; l<m_mtx.height(); ++l) yp << qPow(((long double)l/(long double)(m_mtx.height()-1))-ym,j);
                for (quint32 l=0; l<m_mtx.height(); ++l)
                {
                    long double lsum = 0;
                    T* line = m_mtx.line(l);
                    for (quint32 k=0; k<m_mtx.width(); ++k) if (line[k]!=0.0) lsum += xp[k]*(long double)line[k];
                    lsum*=yp[l];
                    ret+=lsum;
                }
            }
            else
            {
                QPair<double,double> p;

                int k = -1;
                if (m_v.isEmpty()) foreach(p, m_pos)
                {
                    const long double x = p.first, y = p.second;
                    ret = ret + (long double)(qPow((x-xm),i)*qPow((y-ym),j));
                }
                else foreach(p, m_pos)
                {
                    ++k;
                    const long double x = p.first, y = p.second;
                    ret = ret + (long double)m_v[k] * (long double)(qPow((x-xm),i)*qPow((y-ym),j));
                }
            }            
            moment_mu[c]=ret;
        }
        return moment_mu[c];
    }

    /*!
     * \brief Scale invariants
     * \param i
     * \param j
     * \return
     */
    long double getEta(int i, int j)
    {
        Q_ASSERT(i+j>=2);
        QPair<int,int> c(i,j);
        if (!moment_eta.contains(c))
        {
            const long double pwr = 2+i+j, ret = getMu(i,j) / qPow(getMu(0,0), pwr*0.5);
            moment_eta[c]=ret;
        }
        return moment_eta[c];
    }

    /*!
     * \brief Hu moment
     * \param i 1..7
     * \return
     */
    long double getHu(int i)
{
        if (!m_hu.contains(i)) switch(i)
        {
        case 1: // 20+02
                m_hu[i]=getEta(2,0)+getEta(0,2);
        break;
        case 2: // (20-02)^2+4(11)^2
                m_hu[i]=qPow(getEta(2,0)-getEta(0,2),2.0)+4.0*qPow(getEta(1,1),2.0);
        break;
        case 3: // (30-312)^2+(321-03)^2
                m_hu[i]=qPow(getEta(3,0)-3.0*getEta(1,2),2.0)+qPow(3.0*getEta(2,1)-getEta(0,3),2.0);
        break;
        case 4: // (30+12)^2+(21+03)^2
                m_hu[i]=qPow(getEta(3,0)+getEta(1,2),2.0)+qPow(getEta(2,1)+getEta(0,3),2.0);
        break;
        case 5:
        {
            const long double    // (30-312)(30+12)
                            d1 = (getEta(3,0)-3.0*getEta(1,2))*(getEta(3,0)+getEta(1,2)),
                            // (30+12)^2-3(21+03)^2
                            d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-3.0*qPow(getEta(2,1)+getEta(0,3),2.0),
                            // (321-03)(21+03)
                            d3 = (3.0*getEta(2,1)-getEta(0,3))*(getEta(2,1)+getEta(0,3)),
                            // 3(30+12)^2-(21+03)^2
                            d4 = 3.0*qPow(getEta(3,0)+getEta(1,2),2.0)-qPow(getEta(2,1)+getEta(0,3),2.0);
            m_hu[i]= (d1*d2)+(d3*d4);
        }
        break;
        case 6:
        {
            const long double    // (20-02)
                            d1 = getEta(2,0)-getEta(0,2),
                            // (30+12)^2-(21+03)^2
                            d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-qPow(getEta(2,1)+getEta(0,3),2.0),
                            // 411(30+12)(21+03)
                            d3 = 4.0*getEta(1,1)*(getEta(3,0)+getEta(1,2))*(getEta(2,1)+getEta(0,3));
            m_hu[i]=(d1*d2)+d3;
        }
        break;
        case 7:
        {
            const long double
                    //(321-03)(30+12)
                    d1 = (3.0*getEta(2,1)-getEta(0,3)) * (getEta(3,0)+getEta(1,2)),
                    //(30+12)^2-3(21+03)^2
                    d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-3.0*qPow(getEta(2,1)+getEta(0,3),2.0),
                    // (30-312)(21+03)
                    d3 = (getEta(3,0)-3.0*getEta(1,2))*(getEta(2,1)+getEta(0,3)),
                    // (3(30+12)^2-(21+03)^2
                    d4 = 3.0*qPow(getEta(3,0)+getEta(1,2),2.0)-qPow(getEta(2,1)+getEta(0,3),2.0);

            m_hu[i]=(d1*d2)-(d3*d4);
        }
        break;
        default: return 0; break;
    }

    return m_hu[i];
}

    /*!
     * \brief normalized Hu -sign(hu)log(abs(hu))
     * \param i
     * \return
     */
    long double getNHu(int i)
    {
        if (!m_nhu.contains(i))
        {
            const long double hu = getHu(i);
            if (hu==0.0) return 0.0;
            m_nhu[i] = (hu>0) ? -log(qAbs(hu)) : log(qAbs(hu));
        }
        return m_nhu[i];
    }

    void dump()
    {
        QStringList sl;
        QPair<int,int> p;
        for (int i=1; i<8; ++i)        sl << QString("Hu  #%1: %2").arg(i).arg((double)getHu(i));
        for (int i=1; i<8; ++i)        sl << QString("NHu #%1: %2").arg(i).arg((double)getNHu(i));

        sl << QString("Centroid %1 %2").arg((double)xMean()).arg((double)yMean());

        foreach (p, moment_m.  keys()) sl << QString("M  %1%2: %3").arg(p.first).arg(p.second).arg((double)moment_m[p]);
        foreach (p, moment_mu. keys()) sl << QString("Mu %1%2: %3").arg(p.first).arg(p.second).arg((double)moment_mu[p]);
        foreach (p, moment_eta.keys()) sl << QString("Eta%1%2: %3").arg(p.first).arg(p.second).arg((double)moment_eta[p]);
        foreach(const QString& s, sl) qWarning("%s", qPrintable(s));
    }

    inline long double dist(const SScMoments<T>& other)
    {
        long double ret = 0;
        for (int i=1; i<8; ++i) ret +=qAbs(getNHu(i)-other.getNHu(i));
        return ret;
    }
    inline long double edist(const SScMoments<T>& other)
    {
        long double ret = 0;
        for (int i=1; i<8; ++i) ret +=qPow(getNHu(i)-other.getNHu(i),2.0);
        return qSqrt(ret);
    }

private:
    inline long double xMean() { const long double m00 = getM(0,0); return (m00!=0) ? getM(1,0)/m00 : 0.0; }
    inline long double yMean() { const long double m00 = getM(0,0); return (m00!=0) ? getM(0,1)/m00 : 0.0; }

    QVector<QPair<double,double> > normalized(const QVector<QPair<int,int> >& pos)
    {
        QVector<QPair<double,double> > ret;
        if (!pos.isEmpty())
        {
            int xmin=pos.first().first, xmax=xmin, ymin=pos.first().second, ymax=ymin;
            QPair<int,int> p;
            foreach(p, pos)
            {
                if (p.first<xmin) xmin=p.first; else if (p.first>xmax) xmax=p.first;
                if (p.second<ymin) ymin=p.second; else if (p.second>ymax) ymax=p.second;
            }
            const double xscale = xmax>xmin ? 1.0/(xmax-xmin) : 1.0,
                         yscale = ymax>ymin ? 1.0/(ymax-ymin) : 1.0;
            foreach(p,pos)
            {
                const double x = (((double)p.first)-xmin)*xscale,
                             y = (((double)p.second)-ymin)*yscale;
                ret << QPair<double,double>(x,y);
            }
        }
        return ret;
    }


    QMap<QPair<int,int>,long double> moment_m, moment_mu, moment_eta;
    QMap<int,long double> m_hu, m_nhu;
    SScMatrix<T> m_mtx;
    QVector<QPair<double,double> > m_pos;
    QVector<T> m_v;    
};

#endif // MOMENTS_HPP
