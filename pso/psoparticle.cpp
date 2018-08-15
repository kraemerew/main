#include "psoparticle.hpp"
#include <QtMath>

SScPSOParticle::SScPSOParticle() : m_dim(0), m_ftset(false), m_pbestft(0)
{}

SScPSOParticle::SScPSOParticle(quint32 dim, double min, double max, double vperc) : m_dim(dim), m_ftset(false), m_pbestft(0)
{
    Q_ASSERT(dim>0);
    m_p.reserve(dim);
    m_v.reserve(dim);
    init(min,max,vperc);
}

void SScPSOParticle::init(double min, double max, double vperc)
{
    Q_ASSERT(max>min);
    Q_ASSERT(vperc>0);
    m_p.clear(); m_p.reserve(m_dim); for (quint32 i=0; i<m_dim; ++i)  m_p.append(rVal(min,max));
    const double mean = (min+max)/2.0, w = ((max-min)*qBound(0.0, vperc, 100.0))/100;
    min = mean-w;
    max = mean+w;
    m_v.clear(); m_v.reserve(m_dim); for (quint32 i=0; i<m_dim; ++i)  m_v.append(rVal(min,max));
    m_pbest = m_p;
}

double SScPSOParticle::rVal(double min, double max) const { return min+ ((double)qrand()*(max-min)/(double)RAND_MAX); }

QVector<double> SScPSOParticle::get() const { return m_p; }
QVector<double> SScPSOParticle::getBest() const { return m_pbest; }

bool SScPSOParticle::assignFitness(double value)
{
    if ((value>m_pbestft) || !m_ftset)
    {
        m_pbestft = value;
        m_pbest   = m_p;
        m_ftset   = true;
        return true;
    }
    return false;
}

QVector<double> SScPSOParticle::add(const QVector<double>& v1, const QVector<double> &v2, double p1, double p2) const
{
    Q_ASSERT(v1.size()==v2.size());
    QVector<double> ret; ret.reserve(v1.size());
    for (int i=0; i<v1.size(); ++i) ret.append(p1*v1[i]+p2*v2[i]);
    return ret;
}

QVector<double> SScPSOParticle::sub(const QVector<double>& v1, const QVector<double> &v2) const
{
    Q_ASSERT(v1.size()==v2.size());
    QVector<double> ret; ret.reserve(v1.size());
    for (int i=0; i<v1.size(); ++i) ret.append(v1[i]-v2[i]);
    return ret;
}

void SScPSOParticle::move(const QVector<double>& gbest, double cog, double grp)
{
    const QVector<double> difflocal = sub(m_pbest,m_p), diffgrp = sub(gbest,m_p);
    const double p1 = cog*rVal(0,1), p2 = grp*rVal(0,1);
    m_p = add(difflocal,diffgrp,p1,p2);
}

