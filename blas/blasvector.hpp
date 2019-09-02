#ifndef BLASVECTOR_HPP
#define BLASVECTOR_HPP

#include <QVector>

namespace SSnBlas
{
    double vxv(const QVector<double>& a, const QVector<double>& b);
    QVector<double> mxv(const QVector<double>& a, const QVector<double>& b);
}

#endif // BLASVECTOR_HPP
