#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>
#include <QVariantMap>
#include "ssctrainableparameter.hpp"
#include "neuron.hpp"

class SScImageProviderV2;
class SScHighwayNetwork;
class SScConvNeuron;
class SScKernel;

class SSiConvUnit
{
public:
    /*!
     * \brief Constructor
     * \param network   Network
     * \param kern      Kernel size
     * \param stride    Kernel stride
     * \param elements  Output elements
     * \param knr       Kernels to implement
     */
    explicit SSiConvUnit(SScHighwayNetwork* network, const QSize& kern, const QSize& stride, const QSize& elements, int knr = 1);
    virtual ~SSiConvUnit();

    virtual void    reset           ();
    virtual void    resetTraining   ();
    inline  int     kernels         () const { return m_knr; }
    inline  QSize   units           () const { return m_units; }
    inline  QSize   ksize           () const { return m_ksz; }
    inline  QSize   stride          () const { return m_str; }
    virtual int     weights         () const { return m_ksz.width()*m_ksz.height()*depth(); }
    virtual int     depth           () const = 0;
    virtual bool    isColor         () const { return depth()==3; }

    virtual QString type() const = 0;

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);
    static SSiConvUnit* create(SScHighwayNetwork* net, const QVariantMap& vm);

    SScConvNeuron* output(quint32 k, quint32 nidx) const;

protected:
    virtual void clearKernels       ();
    virtual void createKernels      (int);

    SScHighwayNetwork*  m_network;
    QSize               m_ksz, m_str, m_units;
    int                 m_knr;
    QVector<SScKernel*> m_kernels;
};

class SScInputConvUnit : public SSiConvUnit
{
public:
    explicit SScInputConvUnit(SScHighwayNetwork* network, const QSize& kern, const QSize& stride, const QSize& elements, int knr = 1);
    virtual ~SScInputConvUnit();
    QString nextPattern(bool& cycleDone);
    QString addPattern(const QImage& im);
    QString addPattern(const QString& filename);
    virtual SScImageProviderV2* imageProvider() { return m_ip; }
    virtual QString type() const { return "INPUT"; }
    virtual int depth  () const { return 1; }

protected:
    SScImageProviderV2* m_ip;
};

class SScColorInputConvUnit : public SScInputConvUnit
{
public:
    explicit SScColorInputConvUnit(SScHighwayNetwork* network, const QSize& kern, const QSize& stride, const QSize& elements, int knr = 1);
    virtual ~SScColorInputConvUnit();
    virtual int depth  () const { return 3; }
    virtual QString type() const { return "INPUT_COLOR"; }
};

#endif // SSCCONVUNIT_HPP
