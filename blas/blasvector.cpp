#include "blasvector.hpp"
#include <cblas.h>

double SSnBlas::dot(const QVector<double> &a, const QVector<double> &b)
{
    Q_ASSERT(a.size()==b.size());
    return cblas_ddot(a.size(),a.constData(),1,b.constData(),1);
}
