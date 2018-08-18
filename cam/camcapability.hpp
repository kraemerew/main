#ifndef CAMCAPABILITY_HPP
#define CAMCAPABILITY_HPP

#include <QVariantMap>
#include <QStringList>


class SScCamCapability : public QVariantMap
{
public:

    /*!
     * \brief All devices (/dev/video#) found
     * \return
     */
    static QList<quint32> devices();
    /*!
     * \brief All device names (/dev/video#)
     * \return
     */
    static QStringList deviceNames();
    /*!
     * \brief 4vl capaboloty flag to string
     * \param cap
     * \return
     */
    static QString cap2String  (const quint32 cap);
    /*!
     * \brief Construct on /dev/video0
     */
    explicit SScCamCapability();
    /*!
     * \brief Construct on a given device
     * \param device
     */
    explicit SScCamCapability(quint32 device);
    /*!
     * \brief Construct on given device name
     * \param device
     */
    explicit SScCamCapability(const QString& device);

    QString device      () const;
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
    /*!
     * \brief Dump info to shell
     */
    void dump() const;

protected:
    static QString fourcc2String(const quint32 fourcc);
    static quint32 string2Fourcc(const QString& fourcc);

private:
    static QVariantMap init(const QString& device);
};

#endif // CAMCAPABILITY_HPP
