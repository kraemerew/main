#ifndef IMAGEPROVIDER_HPP
#define IMAGEPROVIDER_HPP

#include <QMap>
#include <QHash>
#include <QStringList>
#include <QVector>
#include <QImage>

class SScImageProviderV2
{
public:
    /*!
     * \brief Load and append image
     * \param file Filename
     * \return unique id for this pattern - empty on failure
     */
    QString append(const QString& file);
    /*!
     * \brief Append image
     * \param im
     * \return unique id for this pattern - empty on failure
     */
    QString append(const QImage& im);
    /*!
     * \brief Select next pattern
     * \param endOfCycle    Will be true if end of cycled is reached (used to determine batch end)
     * \return Current uuid
     */
    QString next(bool& endOfCycle);
    /*!
     * \brief Select previous pattern
     * \param endOfCycle    Will be true if end of cycled is reached (used to determine batch end)
     * \return Current uuid
     */
    QString prev(bool& endOfCycle);
    /*!
     * \brief Select pattern by uuid
     * \param uuid
     * \return false on failure
     */
    bool select(const QString& uuid);
    /*!
     * \brief Select first pattern
     * \return
     */
    bool first();
    /*!
     * \brief Select last pattern
     * \return
     */
    bool last();
    /*!
     * \brief Return whether a pattern is activated
     * \return
     */
    inline bool isActive() const { return !m_current.isEmpty(); }
    /*!
     * \brief Return current selected image
     * \return
     */
    inline QImage image() const { return m_data.contains(m_current) ? m_data[m_current]  : QImage(); }

    /*!
     * \brief Return image preprocessed for convolution layer
     * \param kernel    Kernel size
     * \param stride    Kernel stride
     * \param elements  Number of output elements
     * \param color     Color data or grayvalue data
     * \return
     */
    QVector<double> get(const QSize& kernel, const QSize& stride, const QSize& elements, bool color);

private:
    QString calcId(const QString& uuid, const QSize& kernel, const QSize& stride, const QSize& elements, bool color) const;
    inline QImage image(const QSize& sz) const { return m_data.contains(m_current) ? m_data[m_current].scaled(sz,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)  : QImage(); }

    QHash<QString,QImage>           m_data;
    QHash<QString,QVector<double> > m_cached;
    QStringList                     m_seq;
    QString                         m_current, m_last;
};
/*
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
*/
#endif // IMAGEPROVIDER_HPP
