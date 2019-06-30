#ifndef BLASVECTOR_HPP
#define BLASVECTOR_HPP

#include <QVector>

class SScBlasVector : public QVector<double>
{
public:
    double dot(const SScBlasVector& other);
};

#endif // BLASVECTOR_HPP
