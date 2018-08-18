#ifndef CAM_HPP
#define CAM_HPP
#include <QMap>

#include "camcapability.hpp"

class SScCam : public QObject, public SScCamCapability
{
Q_OBJECT
public:
    explicit SScCam(QObject* parent = 0);
    explicit SScCam(quint32 device, QObject* parent = 0);
    explicit SScCam(const QString& device, QObject* parent = 0);

    inline bool isOpen() const  { return m_fd>=0; }
    bool closeStream();
    bool openStream(const QString& fourcc, quint32 w, quint32 h, quint32 buffers = 2);

    void grabFrames(quint32 nr);

private:
    bool queueBuffer();
    bool unqueueBuffer(void**,quint32&);
    bool streamOn();
    bool streamOff();
    void package(void*, quint32);

    int                 m_fd;
    QList<quint32>      m_unqueued, m_queued;
    QMap<void*,quint32> m_buf2idx;
    QMap<quint32,void*> m_idx2buf;
};

#endif // CAM_HPP
