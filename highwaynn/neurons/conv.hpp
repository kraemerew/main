#ifndef HWN_CONV_HPP
#define HWN_GCONV_HPP

#include "neuron.hpp"
#include "ssctrainableparameter.hpp"
class SScKernel;

class SScConvNeuron : public SSiHighwayNeuron
{
public:
    explicit SScConvNeuron(SScHighwayNetwork* net);
    virtual ~SScConvNeuron();
    virtual bool    setInput    (double) { return false; }
    virtual bool    setTarget   (double) { return false; }

    bool setKernel(SScKernel* krn, quint32 idx);
    virtual bool addConnection(SSiHighwayNeuron*, SScTrainableParameter*) { return false; }
    virtual bool addConnection(SSiHighwayNeuron*, double, SScTrainableParameter::Type) { return false; }
    virtual bool delConnection(SSiHighwayNeuron*) { return false; }

    virtual double  net             ();
    virtual double  icon            (SSiHighwayNeuron*);
    virtual void    reset           ();
    virtual double  carry           () { return 0.0; }
    virtual double  highway         () { return 0.0; }
    virtual bool    connectHighway  (SSiHighwayNeuron*, SSiHighwayNeuron*) { return false; }
    virtual double  transform       () { if (m_transformset) return m_t; m_t=m_act->activate(net()); m_transformset=true; return m_t; }
    virtual double  out             () { return transform(); }
    virtual double  deltaw          (SSiHighwayNeuron*);
    virtual double  deltaw(double out);
    //virtual void    trainingStep    ();
    //virtual void    endOfCycle      ();

   // virtual QVariantMap toVM    () const;
    //virtual bool        fromVM  (const QVariantMap &vm);

protected:
    virtual double priv_dedo();

    SScKernel*   m_krn;
    quint32      m_kidx;
};

#endif // CONV_HPP
