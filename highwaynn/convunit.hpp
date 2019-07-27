#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>
#include <QVariantMap>
#include "convpatternprovider.hpp"
#include "ssctrainableparameter.hpp"
#include "neuron.hpp"

class SScHighwayNetwork;
class SScConvNeuron;
class SScKernel;

class SSiConvUnit
{
public:
    explicit SSiConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int knr = 1);
    virtual ~SSiConvUnit();

    virtual int     xunits      () const { return m_unitsx; }
    virtual int     yunits      () const { return m_unitsy; }

    virtual int     xpixels     () const { return m_kx+ ((m_unitsx-1)*(m_kx-m_ovl)); }
    virtual int     ypixels     () const { return m_ky +((m_unitsy-1)*(m_ky-m_ovl)); }
    /*!
     * \brief Number of output units
     * \return
     */
    virtual int     units       () const { return m_unitsx*m_unitsy; }
    /*!
     * \brief Kernel size
     * \return
     */
    virtual int     weights     () const { return m_kx*m_ky*depth(); }
    virtual int     depth       () const { return 1; }
    virtual bool    isColor     () const { return depth()==3; }

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);
    static SSiConvUnit* create(SScHighwayNetwork* net, const QVariantMap& vm);

protected:
    virtual void ensureCleanConf    ();
    virtual void clearKernels       ();
    virtual void createKernels      (int);

    SScHighwayNetwork*  m_network;
    int                 m_kx, m_ky, m_unitsx, m_unitsy, m_ovl;
    QVector<SScKernel*> m_kernels;
};

class SScHiddenConvUnit : public SSiConvUnit
{
public:
    explicit SScHiddenConvUnit(SScHighwayNetwork* network, int kx, int ky, int overlap = 1, int knr = 1);
    virtual ~SScHiddenConvUnit();
    bool connectFrom(SSiConvUnit*);
    virtual QVariantMap toVM() const;
protected:
    SSiConvUnit* m_from;
};

class SScInputConvUnit : public SSiConvUnit
{
public:
    explicit SScInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int knr = 1);
    virtual ~SScInputConvUnit();
    QString nextPattern(bool& cycleDone);
    bool activatePattern(const QString& uuid);
    QString addPattern(const QImage& im) { return m_pp.addPattern(im); }
    QString addPattern(const QString& filename){ return m_pp.addPattern(filename); }

    virtual int depth  () const { return 1; }

    virtual QVariantMap toVM() const;
protected:
    SScConvPatternProvider m_pp;
};

class SScColorInputConvUnit : public SScInputConvUnit
{
public:
    explicit SScColorInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int knr = 1);
    virtual ~SScColorInputConvUnit();
    virtual int depth  () const { return 3; }

    virtual QVariantMap toVM() const;
};

#endif // SSCCONVUNIT_HPP
