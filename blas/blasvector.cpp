#include "blasvector.hpp"
#include <cblas.h>

double SSnBlas::vxv(const QVector<double> &a, const QVector<double> &b)
{
    Q_ASSERT(a.size()==b.size());
    return cblas_ddot(a.size(),a.constData(),1,b.constData(),1);
}

QVector<double> SSnBlas::mxv(const QVector<double> &a, const QVector<double> &b)
{
    Q_ASSERT(a.size()%b.size()==0); // a has multiple size of b - otherwise we cant multiply
    // A: m rows k columns
    // B: k rows n(=1) columns
    // C: m rows n(=1) columns
    const int k = b.size(), m = a.size()/k, n=1;
    QVector<double> c;
    c.fill(0.0,m);
    cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,m,n,k,
                1.0,a.constData(),k,
                b.constData(),n,
                1.0,c.data(),n);
    return c;
}
