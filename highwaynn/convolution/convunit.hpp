#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>
#include <QVariantMap>
#include "convpatternprovider.hpp"
#include "ssctrainableparameter.hpp"
#include "neuron.hpp"

class SScImageProvider;
class SScHighwayNetwork;
class SScConvNeuron;
class SScKernel;

class SSiConvUnit
{
public:
    explicit SSiConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int knr = 1);
    virtual ~SSiConvUnit();

    virtual SScConvPatternProvider* patternProvider() { return NULL; }
    virtual void    reset           ();
    virtual void    resetTraining   ();
    virtual int     kernels         () const { return m_kernels.size(); }
    virtual int     xunits          () const { return m_unitsx; }
    virtual int     yunits          () const { return m_unitsy; }

    virtual int     xpixels         () const { return m_kx+ ((m_unitsx-1)*(m_kx-m_ovl)); }
    virtual int     ypixels         () const { return m_ky +((m_unitsy-1)*(m_ky-m_ovl)); }
    /*!
     * \brief Number of output units
     * \return
     */
    virtual int     units           () const { return m_unitsx*m_unitsy; }
    /*!
     * \brief Kernel size
     * \return
     */
    virtual int     weights         () const { return m_kx*m_ky*depth(); }
    virtual int     depth           () const { return 1; }
    virtual bool    isColor         () const { return depth()==3; }

    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);
    static SSiConvUnit* create(SScHighwayNetwork* net, const QVariantMap& vm);

    SScConvNeuron* output(quint32 k, quint32 nidx) const;

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
    QString addPattern(const QImage& im);
    QString addPattern(const QString& filename);
    virtual SScImageProvider* imageProvider() { return m_ip; }

    virtual int depth  () const { return 1; }

    virtual QVariantMap toVM() const;

protected:
    SScImageProvider* m_ip;
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
