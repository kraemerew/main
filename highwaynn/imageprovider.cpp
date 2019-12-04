#include "imageprovider.hpp"
#include "image/convimageconverter.hpp"
#include <QUuid>
#include "convhelpers.hpp"
QString SScImageProviderV2::append(const QString& file)
{
    return append(QImage(file));
}
QString SScImageProviderV2::append(const QImage& im)
{
    if (!im.isNull())
    {
        m_seq << QUuid::createUuid().toString();
        m_data[m_seq.last()]=im;
        m_current = m_seq.last();
        m_last = m_seq.last();
        return m_current;
    }
    return QString();
}
QString SScImageProviderV2::next(bool& endOfCycle)
{
    endOfCycle=false;
    if (!m_seq.isEmpty())
    {
        m_current = m_seq.takeFirst();
        m_seq.append(m_current);
        endOfCycle = (m_current==m_last);
        return m_current;
    }
    return QString();
}
QString SScImageProviderV2::prev(bool& endOfCycle)
{
    endOfCycle=false;
    if (!m_seq.isEmpty())
    {
        m_current = m_seq.takeLast();
        m_seq.prepend(m_current);
        endOfCycle = (m_current==m_last);
        return m_current;
    }
    return QString();
}
bool SScImageProviderV2::select(const QString &uuid)
{
    if (!m_data.contains(uuid)) return false;
    const QString backup = m_current;
    bool dummy;
    int ctr = 0;
    while ((m_current!=uuid) && (++ctr<=m_seq.size())) next(dummy);
    if (m_current!=uuid) m_current=backup;
    return (m_current==uuid);
}
bool SScImageProviderV2::first()
{
    if (m_data.isEmpty()) return false;
    if (!last()) return false;
    bool dummy;
    next(dummy);
    return true;
}
bool SScImageProviderV2::last()
{
    if (m_data.isEmpty()) return false;
    bool dummy;
    while (m_current!=m_last) next(dummy);
    return true;
}

QVector<double> SScImageProviderV2::get(const QSize& kernel, const QSize& stride, const QSize& elements, bool color)
{
    const QString id = calcId(m_current,kernel,stride,elements,color);
    if (!m_cached.contains(id))
    {
        const auto im = image(SSnConvHelper::inputSize(kernel,stride,elements));
        if (!im.isNull())
        {
            QVector<double> data;
            const auto m = color ? SSnConvHelper::colorValues(SSnConvHelper::matrix(im,kernel,stride,elements))
                                 : SSnConvHelper::grayValues (SSnConvHelper::matrix(im,kernel,stride,elements));
            foreach(auto cline, m) foreach(auto c, cline)
            {
                double v = c;
                v=(v-128.0)/128.0;
                data << v;
            }
            m_cached[id]=data;
        }
    }
    return m_cached.contains(id) ? m_cached[id] : QVector<double>();
}

QString SScImageProviderV2::calcId(const QString& uuid, const QSize &kernel, const QSize &stride, const QSize &elements, bool color) const
{
    const QStringList sl = QStringList() << uuid
                                   << QString("%1").arg(kernel.width()) << QString("%1").arg(kernel.height())
                                   << QString("%1").arg(stride.width()) << QString("%1").arg(stride.height())
                                   << QString("%1").arg(elements.width()) << QString("%1").arg(elements.height())
                                   << (color ? "C":"M");
    return sl.join(" ");
}

SScImageProvider::SScImageProvider(int kx, int ky, int overlap, int outw, int outh, bool color)
  : m_kx    (kx),
    m_ky    (ky),
    m_ovl   (overlap),
    m_outw  (outw),
    m_outh  (outh),
    m_color (color)
{}


QString SScImageProvider::addPattern(const QString &filename)
{
    QImage im;
    if (im.load(filename)) return addPattern(im);
    return QString();
}

QString SScImageProvider::addPattern(const QImage &im)
{
    SScConvImageConverter cic(m_kx,m_ky,m_ovl,m_outw,m_outh,m_color,im);
    if (cic.isValid())
    {
        const auto key = QUuid::createUuid().toString();
        m_patterns[key]=cic.matrix();
        return key;
    }
    return QString();
}

