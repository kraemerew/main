#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>
#include <QVariantMap>
#include "ssctrainableparameter.hpp"
#include "neuron.hpp"

class SScHighwayNetwork;
class SScConvNeuron;

class SSiConvUnit
{
public:
    explicit SSiConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1);
    virtual ~SSiConvUnit();

    inline int      xpixels     () const { return m_kx+ ((m_unitsx-1)*(m_kx-m_ovl)); }
    inline int      ypixels     () const { return m_ky +((m_unitsy-1)*(m_ky-m_ovl)); }
    inline int      units       () const { return m_unitsx*m_unitsy; }
    virtual int     weights     () const { return m_kx*m_ky*depth(); }
    virtual int     neurons     () const { return m_unitsx*m_unitsy; }
    virtual int     depth       () const { return 1; }
    virtual bool    isColor     () const { return depth()==3; }
    virtual double  net         (int idx) const { Q_ASSERT(idx<m_n.size()); return m_n[idx]; }
    virtual QVariantMap toVM() const;
    virtual bool fromVM(const QVariantMap&);

    static SSiConvUnit* create(SScHighwayNetwork* net, const QVariantMap& vm);

protected:
    virtual void ensureCleanConf();
    virtual void clearWeights();
    virtual void createWeights();
    virtual void createNeurons();
    virtual void clearNeurons();

    SScHighwayNetwork*              m_network;
    int                             m_kx, m_ky, m_unitsx, m_unitsy, m_ovl;
    QVector<SScTrainableParameter*> m_w;
    QVector<double>                 m_n;
    QVector<SScConvNeuron*>         m_neurons;
};

class SScHiddenConvUnit : public SSiConvUnit
{
public:
    explicit SScHiddenConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1);
    virtual ~SScHiddenConvUnit();

    virtual QVariantMap toVM() const;
};

class SScInputConvUnit : public SSiConvUnit
{
public:
    explicit SScInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1);
    virtual ~SScInputConvUnit();

    QString addPattern(const QImage& im);    
    QString addPattern(const QString& filename);
    bool activatePattern(const QString& uuid);
    QString nextPattern(bool& cycleDone);
    virtual int depth  () const { return 1; }

    virtual QVariantMap toVM() const;

protected:
    QMap<QString,QImage>                    m_images;
    QMap<QString,QList<QVector<double> > >  m_patterns;
    QList<QString>                          m_pkeys;
};

class SScColorInputConvUnit : public SScInputConvUnit
{
public:
    explicit SScColorInputConvUnit(SScHighwayNetwork* network, int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1);
    virtual ~SScColorInputConvUnit();
    virtual int depth  () const { return 3; }

    virtual QVariantMap toVM() const;
};

#endif // SSCCONVUNIT_HPP
