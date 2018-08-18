#include "cam.hpp"
#include <QTimer>
#include <QByteArray>
#include <QFile>
#include <QDateTime>
#include <QImage>
#include <QCoreApplication>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libv4l2.h>
#include <libv4l2rds.h>
#include <sys/mman.h>
#include "camcapability.hpp"

SScCam::SScCam(quint32 device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers, QObject *parent) : QObject(parent), m_device(QString("/dev/video%1").arg(device)), m_fd(-1), m_son(false)
{
    init(fourcc,w,h,buffers);
}
SScCam::SScCam(const QString &device, const QString& fourcc, quint32 w, quint32 h, quint32 buffers, QObject* parent) : QObject(parent), m_device(device), m_fd(-1), m_son(false)
{
    init(fourcc,w,h,buffers);
}
SScCam::~SScCam()
{
    if (isOpen())
    {
        streamOff();
        close(m_fd);
    }
}

bool SScCam::streamOn()
{
    if (isOpen() && !m_son)
    {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl(m_fd, VIDIOC_STREAMON, &type) < 0) return false;
        m_son=true;
        m_timer.start(10);
        return true;
    }
    return false;
}

bool SScCam::streamOff()
{
    if (isOpen() && m_son)
    {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl(m_fd, VIDIOC_STREAMON, &type) < 0) return false;
        m_son=false;
        m_timer.stop();
        return true;
    }
    return false;
}

bool SScCam::init(const QString &fourcc, quint32 w, quint32 h, quint32 buffers)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timerSlot()), Qt::UniqueConnection);
    SScCamCapability cap(m_device);
    if (!cap.allowed(fourcc,w,h)) return false;

    qWarning("TRYIMNG TO OPEN %s %u x %u", qPrintable(fourcc),w,h);
    if (!isOpen())
    {
        int fd;
        if ((fd = open(m_device.toUtf8().constData(), O_RDWR)) < 0) return false;

        // Set the format
        struct v4l2_format format;
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.pixelformat = SScCamCapability::string2Fourcc(fourcc);
        format.fmt.pix.width = w;
        format.fmt.pix.height = h;

        if(ioctl(fd, VIDIOC_S_FMT, &format) < 0) { close(fd); return false; }

        // Request the buffers
        struct v4l2_requestbuffers bufrequest;
        bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufrequest.memory = V4L2_MEMORY_MMAP;
        bufrequest.count = buffers;

        if(ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0) { close(fd); return false; }


        // Memory map all buffers
        struct v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;

        for (quint32 bufidx=0; bufidx<buffers; ++bufidx)
        {
            bufferinfo.index = bufidx;
            if(ioctl(fd, VIDIOC_QUERYBUF, &bufferinfo) < 0) { close(fd); return false; }

            // MMap
            void* buffer_start = mmap(
                NULL,
                bufferinfo.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                bufferinfo.m.offset
            );

            if(buffer_start == MAP_FAILED) { close(fd); return false; }
            memset(buffer_start, 0, bufferinfo.length);

            m_unqueued << bufidx;
            m_buf2idx[buffer_start]=bufidx;
            m_idx2buf[bufidx]=buffer_start;
        }



        m_fd = fd;
qWarning("OPENED");
        return true;
    }
    return false;
}

bool SScCam::unqueueBuffer(void** ptr, quint32& len)
{
    (*ptr)=NULL;
    len=0;
    if (!m_queued.isEmpty())
    {
        const quint32 idx = m_queued.first();
        struct v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = idx;
        if (ioctl(m_fd, VIDIOC_DQBUF, &bufferinfo) < 0) return false;
        m_queued.pop_front();
        m_unqueued << idx;
        len = bufferinfo.length;
        (*ptr)=m_idx2buf[idx];
        qWarning("Unqueued buffer %u",idx);
        return true;
    }
    return false;
}

bool SScCam::queueBuffer()
{
    if (!m_unqueued.isEmpty())
    {
        const quint32 idx = m_unqueued.first();
        struct v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = idx;
        if (ioctl(m_fd, VIDIOC_QBUF, &bufferinfo) < 0) return false;
        m_unqueued.pop_front();
        m_queued << idx;
        qWarning("Queued buffer %u",idx);
        return true;
    }
    return false;
}

void SScCam::timerSlot()
{
    m_timer.stop();
    if (isOpen() && m_son)
    {
        quint32 len;
        void* ptr;
        // Try to unqueue, if successful then emit the frame
        if (unqueueBuffer(&ptr,len) && ptr && (len>0))
        {
            QImage im;
            if (im.loadFromData(QByteArray::fromRawData((const char*)ptr, len)))
            {
                emit frame(im);
                QString fname = QString("%1.jpg").arg(QDateTime::currentDateTime().toString("hhmmsszzz"));
                if (im.save(fname,"JPG",90)) qWarning("Saved %s", qPrintable(fname));
            }
        }
        // Queue next buffer if possible
        queueBuffer();
        m_timer.start(10);
    }
}
