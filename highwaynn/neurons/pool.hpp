#ifndef HWN_POOL_HPP
#define HWN_POOL_HPP

#include "neuron.hpp"

class SScPoolNeuron : public SSiHighwayNeuron
{
public:
    SScPoolNeuron(SScHighwayNetwork* net, SSiHighwayNeuron::Type t);
    virtual bool    addInput            (SSiHighwayNeuron* other, SScTrainableParameter*);
    virtual bool    addInput            (SSiHighwayNeuron* other, double, SScTrainableParameter::Type);
    virtual bool    delInput            (SSiHighwayNeuron* other);
    virtual bool    connectHighway      (SSiHighwayNeuron*, SSiHighwayNeuron*) { return false; }
    virtual bool    setActivation       (SScActivation::Type, double) { return false; }
    virtual double  icon                (SSiHighwayNeuron*) { return 0.0; }
    virtual double  forwardSelectedDedo (SSiHighwayNeuron* ref);

    virtual double  transform   ();
    virtual double  net         ();
    virtual double  out         ()                  { return transform(); }
    virtual void    reset       ()                  { SSiHighwayNeuron::reset(); m_selected=false; m_sel = NULL; }
    virtual bool    setInput    (double)            { return false; }
    virtual bool    setTarget   (double)            { return false; }
    virtual double  deltag      ()                  { return 0; }
    virtual double  deltaw      (SSiHighwayNeuron*) { return 0; }
    virtual void    trainingStep()                  {}
    virtual void    endOfCycle  ()                  {}

    virtual QVariantMap toVM() const;
    virtual bool        fromVM(const QVariantMap &vm);

    virtual QList<SSiHighwayNeuron*> inputs     () const { return m_in; }
    virtual QList<SSiHighwayNeuron*> allInputs  () const { return inputs(); }

    void dump();

protected:
    virtual void priv_poolselect() = 0;
    virtual double priv_dedo() { return 0; }
    bool                        m_selected;
    SSiHighwayNeuron*           m_sel;                  // The neuron which achieved the maximum
    QList<SSiHighwayNeuron*>    m_in;
};

class SScMaxPoolNeuron : public SScPoolNeuron
{
public:
    SScMaxPoolNeuron(SScHighwayNetwork* net) : SScPoolNeuron(net,MaxPool)
    {}

protected:
    virtual void priv_poolselect()
    {
        m_sel = NULL;
        if (!m_in.isEmpty())
        {
            m_sel = m_in.first();
            double max = m_sel->out();
            for(int i=1; i<m_in.size(); ++i) if (m_in[i]->out()>max)
            {
                m_sel = m_in[i];
                max   = m_sel->out();
            }
        }
    }
};

class SScMinPoolNeuron : public SScPoolNeuron
{
public:
    SScMinPoolNeuron(SScHighwayNetwork* net) : SScPoolNeuron(net,MinPool)
    {}

protected:
    virtual void priv_poolselect()
    {
        m_sel = NULL;
        if (!m_in.isEmpty())
        {
            m_sel = m_in.first();
            double min = m_sel->out();
            for(int i=1; i<m_in.size(); ++i) if (m_in[i]->out()<min)
            {
                m_sel = m_in[i];
                min   = m_sel->out();
            }
        }
    }
};


#endif  // HWN_POOL_HPP
