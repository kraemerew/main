#ifndef BLASVECTOR_HPP
#define BLASVECTOR_HPP

#include <QVector>

namespace SSnBlas
{
    double vxv(const QVector<double>& a, const QVector<double>& b);
    QVector<double> mxv(const QVector<double>& a, const QVector<double>& b);
}
class SScBlasMatrix : QVector<double>
{
public:
    SScBlasMatrix(int c, int r) : m_c(c), m_r(r)
    {
        Q_ASSERT(c*r>0);
        fill(0.00,c*r);
    }
    SScBlasMatrix(int c, int r, const QVector<double>& init) : QVector<double>(init), m_c(c), m_r(r)
    {
        Q_ASSERT(c*r==size());
        if (size()!=c*r) fill(0.00,c*r);
    }
    QVector<double> operator * (const QVector<double> v)
    {
        QVector<double> ret;
        if (v.size()==m_c) ret = SSnBlas::mxv(*this,v);
        return ret;
    }
    inline int r() const { return m_r; }
    inline int c() const { return m_c; }

    inline double* at(int r) { return data()+(r*m_c); }
    inline double& at(int c, int r) { return *(at(r)+c); }
    void dump()
    {
        for (int r=0; r<m_r; ++r)
        {
            for (int c=0; c<m_c; ++c) printf(" %lf",at(c,r));
            printf("\n");
        }
    }
private:
    int m_c, m_r;
};

#endif // BLASVECTOR_HPP
