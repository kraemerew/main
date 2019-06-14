#include "moments.hpp"
#include <QtMath>

template<typename T> SScMoments<T>::SScMoments(const SScMatrix<T> &m) : m_mtx(m)
{

}

template<typename T> SScMoments<T>::SScMoments(const QVector<QPair<int,int> >& pos, const QVector<T>& v) : m_pos(pos), m_v(v)
{
    if (!v.isEmpty())
    {
        Q_ASSERT(pos.size()==v.size());
    }
}

template<typename T> double SScMoments<T>::getM(int i, int j)
{
    QPair<int,int> c(i,j);
    if (!moment_m.contains(c))
    {
        double ret = 0;
        if (!m_mtx.isEmpty())
        {

        }
        else
        {
            QPair<int,int> p;

            int k = -1;
            if (m_v.isEmpty()) foreach(p, m_pos)
            {
                const int x = p.first, y = p.second;
                ret = ret + (double)(qPow(x,i)*qPow(y,j));
            }
            else foreach(p, m_pos)
            {
                ++k;
                const int x = p.first, y = p.second;
                ret = ret + (double)m_v[k] * (double)(qPow(x,i)*qPow(y,j));
            }
        }
        moment_m[c]=ret;
    }
    return moment_m[c];
}

template<typename T> double SScMoments<T>::getMu(int i, int j)
{

    QPair<int,int> c(i,j);
    if (!moment_mu.contains(c))
    {

        double xm = xMean(), ym = yMean(), ret = 0;
        if (!m_mtx.isEmpty())
        {

        }
        else
        {
            QPair<int,int> p;

            int k = -1;
            if (m_v.isEmpty()) foreach(p, m_pos)
            {
                const double x = p.first, y = p.second;
                ret = ret + (double)(qPow((x-xm),i)*qPow((y-ym),j));
            }
            else foreach(p, m_pos)
            {
                ++k;
                const double x = p.first, y = p.second;
                ret = ret + (double)m_v[k] * (double)(qPow((x-xm),i)*qPow((y-ym),j));
            }
        }
        moment_mu[c]=ret;
    }
    return moment_mu[c];
}

template<typename T> double SScMoments<T>::getEta(int i, int j)
{

    QPair<int,int> c(i,j);
    if (!moment_eta.contains(c))
    {
        const double pwr = 2+i+j, ret = getMu(i,j) / qPow(getMu(0,0), pwr*0.5);
        moment_eta[c]=ret;
    }
    return moment_eta[c];
}

template<typename T> double SScMoments<T>::getHu(int i)
{
    if (!m_hu.contains(i)) switch(i)
    {
        case 1: m_hu[i]=getEta(2,0)+getEta(0,2); break;
        case 2: m_hu[i]=qPow(getEta(2,0)-getEta(0,2),2.0)+4*qPow(getEta(1,1));  break;
        case 3: m_hu[i]=qPow(getEta(3,0)-3.0*getEta(1,2),2.0)+qPow(3.0*getEta(2,1)-getEta(0,3),2.0); break;
        case 4: m_hu[i]=qPow(getEta(3,0)+getEta(1,2),2.0)+qPow(getEta(2,1)+getEta(0,3),2.0); break;
        case 5:
        {
            const double    d1 = (getEta(3,0)-3.0*getEta(1,2))*(getEta(3,0)+getEta(1,2)),
                            d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-3.0*qPow(getEta(1,2)+getEta(0,3),2.0),
                            d3 = (3.0*getEta(2,1)-getEta(0,3))*(getEta(2,1)+getEta(0,3)),
                            d4 = 3.0*qPow(getEta(0,3)+getEta(1,2),2.0)-qPow(getEta(2,1)-getEta(0,3),2.0);
            m_hu[i]= (d1*d2)+(d3*d4);
        }
        break;
        case 6:
        {
            const double    d1 = getEta(2,0)-getEta(0,2),
                            d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-qPow(getEta(2,1)+getEta(0,3),2.0),
                            d3 = 4.0*getEta(1,1)*(getEta(3,0)+getEta(1,2))*(getEta(2,1)+getEta(0,3));
            m_hu[i]=(d1*d2)+d3;
        }
        break;
        case 7:
        {
            const double    d1 = (3.0*getEta(2,1)-getEta(0,3)) * (getEta(3,0)+getEta(1,2)),
                            d2 = qPow(getEta(3,0)+getEta(1,2),2.0)-3.0*qPow(getEta(2,1)+getEta(0,3),2.0),
                            d3 = (getEta(3,0)-3.0*getEta(1,2))*(getEta(2,1)+getEta(0,3)),
                            d4 = 3.0*qPow(getEta(3,0)+getEta(1,2),2.0)-qPow(getEta(2,1)+getEta(0,3),2.0);
            m_hu[i]=(d1*d2)-(d3*d4);
        }
        break;
    }

    return m_hu[i];
}

