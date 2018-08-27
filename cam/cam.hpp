#ifndef CAM_HPP
#define CAM_HPP
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QTimer>
#include "frameintervaldescriptor.hpp"
#include "framereader.hpp"

class SScCam : public QObject
{
Q_OBJECT
public:
    explicit SScCam(quint32 device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers = 2, QObject* parent = 0);
    explicit SScCam(const QString& device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers = 2, QObject* parent = 0);
    virtual ~SScCam();

    inline bool isOpen() const  { return m_fd>=0; }
    /*!
     * \brief Switch stream on
     * \param fps Frames per second - leave 0 to not change the parameter
     * \return
     */
    bool streamOn(int fps = 0);
    bool streamOff();

signals:
    void frame(const QImage&);

private slots:
    void timerSlot();

private:
    bool init(const QString& fourcc, quint32 w, quint32 h, quint32 buffers);
    bool queueBuffer();
    bool unqueueBuffer(void**,quint32&);
    void package(void*, quint32);

    QString                             m_device;
    int                                 m_fd;
    bool                                m_son;
    QList<quint32>                      m_unqueued, m_queued;
    QMap<void*,quint32>                 m_buf2idx;
    QMap<quint32,void*>                 m_idx2buf;
    QMap<void*,quint32>                 m_buf2len;
    QTimer                              m_timer;
    QList<SScFrameIntervalDescriptor>   m_fid;
    SScFrameReaderBase*                 m_framereader;
};

#endif // CAM_HPP
