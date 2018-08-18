#ifndef CAM_HPP
#define CAM_HPP
#include <QStringList>
#include <QVariantMap>

namespace SSnCam
{
QList<int> devices();
QStringList deviceNames();


}



class SScCamCapability : public QVariantMap
{
public:
    explicit SScCamCapability();
    explicit SScCamCapability(int device);
    explicit SScCamCapability(const QString& device);

    QString cap2String(const quint32 cap) const;
    QString driver() const;
    QString card() const;
    QString busInfo() const;
    quint32 version() const;
    quint32 caps() const;
    quint32 devcaps() const;

    quint32 formats() const;
    quint32 formatFlags(int fidx) const;
    bool    formatEmulated(int fidx) const;
    bool    formatCompressed(int fidx) const;
    QString formatDescriptor(int fidx) const;

    QString formatPixelFormat(int fidx) const;



    void dump() const;

private:
static QVariantMap init(const QString& device);
bool m_ok;
static QString fourcc2String(const quint32 pixelformat);
};

#endif // CAM_HPP
