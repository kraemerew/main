#ifndef PSOSWARM_HPP
#define PSOSWARM_HPP
#include "psoparticle.hpp"

class SSiSwarmInterface
{
public:
    virtual quint32 dim     () const = 0;
    virtual quint32 size    () const { return 100; }
    virtual double  min     () const { return -1; }
    virtual double  max     () const { return  1; }
    virtual double  vperc   () const { return  1; }
    virtual double  cog     () const { return  1.1; }
    virtual double  grp     () const { return  0.9; }

    virtual double  fitness (const SScPSOParticle& p) const = 0;
};

class SScPSOSwarm
{
public:
    SScPSOSwarm(SSiSwarmInterface* ifc);
    bool step();
    QVector<double> best() const { return m_gbest; }
    double bestFt() const { return m_gbestft; }

private:
    SSiSwarmInterface*      m_ifc;
    double                  m_gbestft;
    bool                    m_gbestftset;
    QVector<double>         m_gbest;
    QVector<SScPSOParticle> m_swarm;
};

#endif // PSOSWARM_HPP
