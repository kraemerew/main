#ifndef PSOPARTICLE_HPP
#define PSOPARTICLE_HPP

#include <QVector>
class SScPSOParticle
{
public:
    SScPSOParticle();
    SScPSOParticle(quint32 dim, double min, double max = 1, double vperc = 1);
    QVector<double> get() const;
    QVector<double> getBest() const;

    void move(const QVector<double>& gbest, double, double);
    bool assignFitness(double value);

private:
    double          rVal    (double, double) const;
    void            init    (double,double,double);
    QVector<double> add     (const QVector<double>& v1, const QVector<double> &v2, double p1, double p2) const;
    QVector<double> sub     (const QVector<double>& v1, const QVector<double> &v2) const;

    quint32         m_dim;
    bool            m_ftset;
    double          m_pbestft;
    QVector<double> m_p, m_pbest, m_v;
};

#endif // PSOPARTICLE_HPP
