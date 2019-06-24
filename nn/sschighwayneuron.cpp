#include "sschighwayneuron.hpp"
#include "sschighwaynetwork.hpp"


SScHighwayNeuron::SScHighwayNeuron(SScHighwayNetwork* nw, int layer, int nr)
    : SScNeuron (SScNeuron::NeuronType_Hidden),
      m_nw      (nw),
      m_layer   (layer),
      m_nr      (nr)
{
    Q_CHECK_PTR(nw);
}


double SScHighwayNeuron::out()
{
    const double tout  = m_nw->transformNeuron(m_layer)->out(),
                 carry = 1.0-tout;
    return tout *m_act->activate(net())
          +carry*m_nw->neuron(m_layer-1,m_nr)->out();
}

bool SScHighwayNeuron::addInput(SScNeuron *other, double v)
{
    Q_CHECK_PTR(other);
    if ((this==other) || m_in.contains(other)) return false;
    m_in[other]=QSharedPointer<SScTrainableParameter>(SScTrainableParameter::create(SScTrainableParameter::CON_RPROP,v));
    return true;
}

bool SScHighwayNeuron::delInput(SScNeuron *other)
{
    Q_CHECK_PTR(other);
    if (!m_in.contains(other)) return false;
    m_in.remove(other);
    return true;
}

double SScHighwayNeuron::net()
{
    double ret = 0;
    for(QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        ret += it.key()->out()*it.value()->value();
    return ret;
}

double SScHighwayNeuron::deltag()
{
    return -dedo()*net()*m_act->dev();
}

double SScHighwayNeuron::deltaw(SScNeuron* n)
{
    //TODO
    // dE/dw_ij=dE/doj*doj/dw_ij=dedo()*doj/dnetj*dnetj/dwij
    // = dedo()*doj/dnetj*oi

    return -dedo()*n->out()*m_act->dev()*m_act->gain();
}


bool SScHighwayNeuron::trainingStep(bool cycleDone)
{
    for(QMap<SScNeuron*,QSharedPointer<SScTrainableParameter> >::iterator it = m_in.begin(); it != m_in.end(); ++it)
        it.value()->update(deltaw(it.key()),cycleDone);
    m_act->updateGain(deltag(),cycleDone);
    return true;
}
