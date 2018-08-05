#include "psoswarm.hpp"

SScPSOSwarm::SScPSOSwarm(SSiSwarmInterface* ifc) : m_ifc(ifc), m_gbestft(0), m_gbestftset(false)
{
    Q_CHECK_PTR(ifc);
    m_swarm.reserve(ifc->size());
    while (m_swarm.size()<(int)m_ifc->size()) m_swarm.append(SScPSOParticle(ifc->dim(),ifc->min(),ifc->max(),ifc->vperc()));
}


bool SScPSOSwarm::step()
{
    bool ret = false;
    for (int i=0; i<m_swarm.size(); ++i)
    {
        const double ft = m_ifc->fitness(m_swarm[i]);
        (void) m_swarm[i].assignFitness(ft);
        if ((ft>m_gbestft) || !m_gbestftset)
        {
            m_gbestftset = true;
            m_gbest = m_swarm[i].get();
            m_gbestft = ft;
            ret = true;
        }
    }
    for (int i=0; i<m_swarm.size(); ++i)
    {
        m_swarm[i].move(m_gbest);
    }
    return ret;
}
