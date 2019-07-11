#ifndef SSCCONVUNIT_HPP
#define SSCCONVUNIT_HPP

#include <QMap>
#include <QImage>
#include <QVector>

class SScConvUnit
{
public:
    SScConvUnit(int kx, int ky, int unitsx=8, int unitsy = 8, int overlap = 1, int pooling = 2, bool color = false);

    inline int  xpixels() const { return m_kx+ ((m_unitsx-1)*(m_kx-m_ovl)); }
    inline int  ypixels() const { return m_ky +((m_unitsy-1)*(m_ky-m_ovl)); }
    inline int  units  () const { return m_unitsx*m_unitsy; }
    inline bool pooling() const { return m_pooling>1; }
    QString addPattern(const QImage& im);

    bool activatePattern(const QString& uuid);
    QString nextPattern();

protected:
    int                                     m_kx, m_ky, m_unitsx, m_unitsy, m_ovl, m_pooling;
    bool                                    m_color;
    QMap<QString,QImage>                    m_images;
    QMap<QString,QList<QVector<double> > >  m_patterns;
    QList<QString>                          m_pkeys;
    QVector<double>                         m_w, m_n;
};

#endif // SSCCONVUNIT_HPP
