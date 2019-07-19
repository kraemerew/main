#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>
#include <QVariantMap>
#include "ssctrainableparameter.hpp"

class SScConvUnit
{
public:
    explicit SScConvUnit(int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int pooling = 2, bool color = false);
    virtual ~SScConvUnit();

    inline int  xpixels() const { return m_kx+ ((m_unitsx-1)*(m_kx-m_ovl)); }
    inline int  ypixels() const { return m_ky +((m_unitsy-1)*(m_ky-m_ovl)); }
    inline int  units  () const { return m_unitsx*m_unitsy; }
    inline int  weights() const { return m_kx*m_ky; }
    inline bool pooling() const { return m_pooling>1; }

    QString addPattern(const QImage& im);

    bool activatePattern(const QString& uuid);
    QString nextPattern();

    QVariantMap toVM() const;
    bool fromVM(const QVariantMap&);

protected:
    void ensureCleanConf(bool clear);
    int                                     m_kx, m_ky, m_unitsx, m_unitsy, m_ovl, m_pooling;
    bool                                    m_color;
    QMap<QString,QImage>                    m_images;
    QMap<QString,QList<QVector<double> > >  m_patterns;
    QList<QString>                          m_pkeys;
    QVector<SScTrainableParameter*>         m_w;
    QVector<double>                         m_n,            // < net
                                            m_npooled;
};

#endif // SSCCONVUNIT_HPP
