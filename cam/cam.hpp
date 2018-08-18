#ifndef CAM_HPP
#define CAM_HPP
#include <QStringList>
#include <QVariantMap>

class SScCamCapability : public QVariantMap
{
public:


    static QList<quint32> devices();
    static QStringList deviceNames();



    explicit SScCamCapability();
    explicit SScCamCapability(quint32 device);
    explicit SScCamCapability(const QString& device);

    QString device      () const;
    QString cap2String  (const quint32 cap) const;
    QString driver      () const;
    QString card        () const;
    QString busInfo     () const;
    quint32 version     () const;
    quint32 caps        () const;
    quint32 devcaps     () const;

    quint32     formats         () const;
    quint32     formatFlags     (const QString& fourcc) const;
    bool        formatEmulated  (const QString& fourcc) const;
    bool        formatCompressed(const QString& fourcc) const;
    QString     formatDescriptor(const QString& fourcc) const;
    QStringList frameSizes      (const QString& fourcc) const;

    /*!
     * \brief Return the format id (fourcc encoded) for a given index
     * \param fidx
     * \return
     */
    QString index2Format(quint32 fidx) const;
    /*!
     * \brief Return index for a given format (fourcc encoded, for instance MJPEG)
     * \param fourcc
     * \return
     */
    quint32 format2Index(const QString& fourcc) const;
    /*!
     * \brief List of fourcc encoded format IDs
     * \return
     */
    QStringList formatList() const;

    /*!
     * \brief Check whether a pixel format is allowed at a given resolution
     * \param fourcc
     * \param w
     * \param h
     * \return
     */
    bool allowed(const QString& fourcc, quint32 w, quint32 h) const;

    void dump() const;

protected:
    static QString fourcc2String(const quint32 fourcc);
    static quint32 string2Fourcc(const QString& fourcc);

private:
    static QVariantMap init(const QString& device);
};




class SScCam : public QObject, public SScCamCapability
{
public:
    explicit SScCam(QObject* parent = 0);
    explicit SScCam(quint32 device, QObject* parent = 0);
    explicit SScCam(const QString& device, QObject* parent = 0);

inline bool isOpen() const  { return m_fd>=0; }
    bool openStream(const QString& fourcc, quint32 w, quint32 h);
private:
    int m_fd;
};



#endif // CAM_HPP
