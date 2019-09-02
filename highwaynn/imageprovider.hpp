#ifndef IMAGEPROVIDER_HPP
#define IMAGEPROVIDER_HPP

#include <QMap>
#include <QStringList>
#include <QVector>
#include <QImage>

class SScImageProvider
{
public:
    SScImageProvider(int kx, int ky, int overlap, int outw, int outh, bool color);


    QString nextKey(bool& endOfCycle)
    {
        endOfCycle=false;
        if (m_pkeys.isEmpty()) return QString();
        const QString key = m_pkeys.takeFirst();
        m_pkeys << key;
        if (key==m_patterns.lastKey()) endOfCycle=true;
        return key;
    }
    QString addPattern(const QString& filename);
    QString addPattern(const QImage &im);

    inline QVector<double> pattern(const QString& key) const { return m_patterns.contains(key) ? m_patterns[key]:QVector<double>(); }

private:
    int     m_kx, m_ky, m_ovl, m_outw, m_outh;
    bool    m_color;

    QStringList                     m_pkeys;
    QMap<QString,QVector<double> >  m_patterns;
};

#endif // IMAGEPROVIDER_HPP
