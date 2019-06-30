#include "blasvector.hpp"
#include <cblas.h>

double SScBlasVector::dot(const SScBlasVector &other)
{
    Q_ASSERT(size()==other.size());
    return cblas_ddot(size(),constData(),1,other.constData(),1);
}
