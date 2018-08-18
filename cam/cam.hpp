#ifndef CAM_HPP
#define CAM_HPP
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QTimer>

class SScCam : public QObject
{
Q_OBJECT
public:
    explicit SScCam(quint32 device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers = 2, QObject* parent = 0);
    explicit SScCam(const QString& device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers = 2, QObject* parent = 0);
    virtual ~SScCam();

    inline bool isOpen() const  { return m_fd>=0; }
    bool streamOn();
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

    QString             m_device;
    int                 m_fd;
    bool                m_son;
    QList<quint32>      m_unqueued, m_queued;
    QMap<void*,quint32> m_buf2idx;
    QMap<quint32,void*> m_idx2buf;
    QTimer              m_timer;
};

#endif // CAM_HPP
