#ifndef CONVPATTERNPROVIDER_HPP
#define CONVPATTERNPROVIDER_HPP

#include <QMap>
#include <QList>
#include <QImage>

class SScConvPatternProvider
{
public:
    SScConvPatternProvider()
    {
        reconfigure(9,9,false,3,3,0);
    }
    SScConvPatternProvider(int x, int y, bool c, int kx, int ky, int ovl)
    {
        reconfigure(x,y,c,kx,ky,ovl);
    }
    QString nextKey(bool& endOfCycle)
    {
        endOfCycle=false;
        if (m_pkeys.isEmpty()) return QString();
        const QString key = m_pkeys.takeFirst();
        m_pkeys << key;
        if (key==m_patterns.lastKey()) endOfCycle=true;
        return key;
    }
    void reconfigure(int x, int y, bool c, int kx, int ky, int ovl);

    inline bool hasPattern(const QString& key) { return m_patterns.contains(key); }
    inline void clear()
    {
        m_images.clear();
        m_patterns.clear();
        m_pkeys.clear();
    }
    QString addPattern(const QImage& im);
    QString addPattern(const QString& filename);
    QVector<QVector<double> > getPattern(const QString& key) const
    {
        if (m_patterns.contains(key)) return m_patterns[key];
        return QVector<QVector<double> >();
    }

private:
    int                                     m_xres, m_yres, m_kx, m_ky, m_ovl;  // Kernel settings
    bool                                    m_isColor;
    QMap<QString,QImage>                    m_images;   // Source images for each pattern
    QMap<QString,QVector<QVector<double> > >m_patterns; // Per pattern: the pattern vectors for each of the output neurons
    QList<QString>                          m_pkeys;    // Keeps track of training sequence - current pattern at beginning
};

#endif // CONVPATTERNPROVIDER_HPP
