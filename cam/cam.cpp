#include "cam.hpp"
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QByteArray>
#include <QFile>
#include <QDateTime>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libv4l2.h>
#include <libv4l2rds.h>
#include <sys/mman.h>

class SScResolutionChecker
{
public:
    explicit SScResolutionChecker(const QStringList& sl) : m_minw(0), m_minh(0), m_maxw(0), m_maxh(0), m_dltw(0), m_dlth(0)
    {
        qWarning("SSC RESOLUTION CHECK");
        if (sl.size()==1)
        {
            // Continuous or stepwise
            QStringList nl = sl.first().split("-");
            auto p1 = QPair<quint32,quint32>(0,0),
                 p2 = p1, p3 = p1;
            if (!nl.isEmpty()) p1 = s2p(nl.takeFirst());
            if (!nl.isEmpty()) p2 = s2p(nl.takeFirst());
            if (!nl.isEmpty()) p3 = s2p(nl.takeFirst());
            if (pValid(p1) && pValid(p2) && pValid(p3))
            {
                m_minw = p1.first;
                m_minh = p1.second;
                m_maxw = p2.first;
                m_maxh = p2.second;
                m_dltw = p3.first;
                m_dlth = p3.second;
            }
        }
        else foreach(const QString& s, sl)
        {
            // Discrete resolutions
            const auto p = s2p(s);
            if (pValid(p)) m_allowed << p;
        }
    }
    bool pValid(const QPair<quint32,quint32>& p) const
    {
        return (p.first>0) & (p.second>0);
    }
    QPair<quint32,quint32> s2p(const QString& s) const
    {
        QStringList nl = s.split("x");
        bool ok1 = false, ok2 = false;
        int w = 0, h = 0;
        if (!nl.isEmpty()) w = nl.takeFirst().toInt(&ok1);
        if (!nl.isEmpty()) h = nl.takeFirst().toInt(&ok2);
        return (ok1&&ok2) ? QPair<quint32,quint32>(w,h) : QPair<quint32,quint32>(0,0);
    }

    bool stepWise() const { return (m_minw>0) && (m_minh>0) && (m_maxw>m_minw) && (m_maxh>m_minh) && (m_dltw>0) && (m_dlth>0); }
    bool allowed(quint32 w, quint32 h) const
    {
        if (m_allowed.contains(QPair<quint32,quint32>(w,h))) return true;
        quint32 cw = m_minw, ch = m_minh;
        if (stepWise()) do
        {
            if ((cw==w) && (ch==h)) return true;
            if ((cw>w) || (ch>h)) return false;
            cw+=m_dltw;
            ch+=m_dlth;
            if ((cw>m_maxw) || (ch>m_maxh)) return false;
        }
        while (true);
        return false;
    }

    QSet<QPair<quint32,quint32> > m_allowed;
    quint32 m_minw, m_minh, m_maxw, m_maxh, m_dltw, m_dlth;
};

SScCamCapability::SScCamCapability() : QVariantMap(init(QString("/dev/video0")))
{}

SScCamCapability::SScCamCapability(quint32 device) : QVariantMap(init(QString("/dev/video%1").arg(device)))
{}

SScCamCapability::SScCamCapability(const QString& device) : QVariantMap(init(device))
{}

QString SScCamCapability::fourcc2String(const quint32 fourcc)
{
    const uchar a = (fourcc      )&0xff,
                b = (fourcc >>  8)&0xff,
                c = (fourcc >> 16)&0xff,
                d = (fourcc >> 24)&0xff;
    QString ret;
    ret+=a;
    ret+=b;
    ret+=c;
    ret+=d;
    return ret;
}
quint32 SScCamCapability::string2Fourcc(const QString& fourcc)
{
    quint32 ret = 0u;
    if (fourcc.length()==4)
    {
        const QByteArray ba = fourcc.toUtf8();
        ret |= ba[3]; ret <<= 8;
        ret |= ba[2]; ret <<= 8;
        ret |= ba[1]; ret <<= 8;
        ret |= ba[0];
    }
    return ret;
}

bool SScCamCapability::allowed(const QString &fourcc, quint32 w, quint32 h) const
{
    if (formatList().contains(fourcc))
    {
        return SScResolutionChecker(frameSizes(fourcc)).allowed(w,h);
    }
    return false;
}

QVariantMap SScCamCapability::init(const QString& dev)
{
    int fd;
    if ((fd = open(dev.toUtf8().constData(), O_RDWR)) < 0) return QVariantMap();
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { close(fd); return QVariantMap(); }

    QVariantMap ret;
    ret["DEVICE"]=dev;

    // Capabilities
    QByteArray ba0, ba1, ba2;
    for (int i=0; i<16; ++i) ba0.append(cap.driver[i]);
    for (int i=0; i<32; ++i) ba1.append(cap.card[i]);
    for (int i=0; i<32; ++i) ba2.append(cap.bus_info[i]);

    ret["DRIVER"]   = QString(ba0);
    ret["CARD"]     = QString(ba1);
    ret["BUSINFO"]  = QString(ba2);
    ret["VERSION"]  = cap.version;
    ret["CAPS"]     = cap.capabilities;
    ret["DEVCAPS"]  = cap.device_caps;

QList<quint32> pxfmts;
    // Format
    bool go_on = true;
    struct v4l2_fmtdesc fmt;
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    do
    {
        if (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) < 0) go_on = false;
        else
        {
            const QString fourcc = fourcc2String(fmt.pixelformat), prefix = QString("FORMAT_%1").arg(fourcc);
            ret[prefix+"_INDEX"] = fmt.index;
            QByteArray ba; for (int i=0; i<32; ++i) ba.append(fmt.description[i]);
            ret[prefix+"_DESCRIPTOR"]  = QString(ba);
            ret[prefix+"_FLAGS"]        = fmt.flags;
            const bool compressed = fmt.flags&V4L2_FMT_FLAG_COMPRESSED,
                       emulated   = fmt.flags&V4L2_FMT_FLAG_EMULATED;
            ret[prefix+"_COMPRESSED"] = compressed;
            ret[prefix+"_EMULATED"]   = emulated;
            pxfmts << fmt.pixelformat;

            ++fmt.index;
            if (fmt.index==0) go_on = false;    //< only theoretically possible
        }

    } while (go_on);

    QStringList sl;
    foreach(quint32 pxfmt, pxfmts) sl << fourcc2String(pxfmt);
    ret["FORMAT_LIST"] = sl.join(",");


    foreach(quint32 pxfmt, pxfmts)
    {
        struct v4l2_frmsizeenum frmsz;
        frmsz.index=0;
        frmsz.pixel_format=pxfmt;
        go_on = true;
        do
        {
            if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsz) < 0) go_on = false;
            else
            {
                QString token = QString("FRAMESIZE_%1").arg(fourcc2String(pxfmt)), value;
                switch(frmsz.type)
                {
                    case V4L2_FRMSIZE_TYPE_DISCRETE:
                    value = QString("%1 x %2")
                            .arg(frmsz.discrete.width)
                            .arg(frmsz.discrete.height);
                    break;
                    case V4L2_FRMSIZE_TYPE_CONTINUOUS:
                    case V4L2_FRMSIZE_TYPE_STEPWISE:
                    value = QString("%1 x %2 - %3 x %4 - %5 x %6")
                            .arg(frmsz.stepwise.min_width)
                            .arg(frmsz.stepwise.max_width)
                            .arg(frmsz.stepwise.min_height)
                            .arg(frmsz.stepwise.max_height)
                            .arg(frmsz.stepwise.step_width)
                            .arg(frmsz.stepwise.step_height);
                    break;
                }

                if (!value.isEmpty())
                {
                    qWarning("SUCCESS FRAMESIZE %s = %s", qPrintable(token), qPrintable(value));
                    if (!ret[token].toString().isEmpty()) ret[token] = ret[token].toString()+", "+value;
                    else ret[token]=value;
                }
                ++frmsz.index;
            }
        }
        while (go_on);
    }

    close(fd);
    return ret;
}

QString SScCamCapability::cap2String(const quint32 cap) const
{
    switch(cap)
    {
        case V4L2_CAP_VIDEO_CAPTURE:            return "Is a video capture device";
        case V4L2_CAP_VIDEO_OUTPUT:             return "Is a video output device";
        case V4L2_CAP_VIDEO_OVERLAY:            return "Can do video overlay";
        case V4L2_CAP_VBI_CAPTURE:              return "Is a raw VBI capture device";
        case V4L2_CAP_VBI_OUTPUT:               return "Is a raw VBI output device";
        case V4L2_CAP_SLICED_VBI_CAPTURE:       return "Is a sliced VBI capture device";
        case V4L2_CAP_SLICED_VBI_OUTPUT:        return "Is a sliced VBI output device";
        case V4L2_CAP_RDS_CAPTURE:              return "RDS data capture";
        case V4L2_CAP_VIDEO_OUTPUT_OVERLAY:     return "Can do video output overlay";
        case V4L2_CAP_HW_FREQ_SEEK:             return "Can do hardware frequency seek ";
        case V4L2_CAP_RDS_OUTPUT:               return "Is an RDS encoder";
        case V4L2_CAP_VIDEO_CAPTURE_MPLANE:     return "Is a video capture device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_OUTPUT_MPLANE:      return "Is a video output device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_M2M_MPLANE:         return "Is a video mem-to-mem device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_M2M:                return "Is a video mem-to-mem device";
        case V4L2_CAP_TUNER:                    return "has a tuner";
        case V4L2_CAP_AUDIO:                    return "has audio support";
        case V4L2_CAP_RADIO:                    return "is a radio device";
        case V4L2_CAP_MODULATOR:                return "has a modulator";
        case V4L2_CAP_SDR_CAPTURE:              return "Is a SDR capture device";
        case V4L2_CAP_EXT_PIX_FORMAT:           return "Supports the extended pixel format";
        case V4L2_CAP_SDR_OUTPUT:               return "Is a SDR output device";
        case V4L2_CAP_META_CAPTURE:             return "Is a metadata capture device";
        case V4L2_CAP_READWRITE:                return "read/write systemcalls";
        case V4L2_CAP_ASYNCIO:                  return "async I/O";
        case V4L2_CAP_STREAMING:                return "streaming I/O ioctls";
        case V4L2_CAP_TOUCH:                    return "Is a touch device";
        case V4L2_CAP_DEVICE_CAPS:              return "sets device capabilities field";
        default: return QString("Unknown capability %1").arg(cap);
    }

}
QString     SScCamCapability::device            () const { return (*this)[ "DEVICE"].toString(); }
QString     SScCamCapability::driver            () const { return (*this)[ "DRIVER"].toString(); }
QString     SScCamCapability::card              () const { return (*this)[   "CARD"].toString(); }
QString     SScCamCapability::busInfo           () const { return (*this)["BUSINFO"].toString(); }
quint32     SScCamCapability::version           () const { return (*this)["VERSION"].toUInt  (); }
quint32     SScCamCapability::caps              () const { return (*this)[   "CAPS"].toUInt  (); }
quint32     SScCamCapability::devcaps           () const { return (*this)["DEVCAPS"].toUInt  (); }
quint32     SScCamCapability::formats           () const { return formatList().size();  }

QString     SScCamCapability::index2Format      (quint32 fidx) const            { const QStringList sl = formatList(); return ((quint32)sl.size()>fidx) ? sl[fidx] : QString(); }
quint32     SScCamCapability::format2Index      (const QString& fourcc) const   { return (*this)[QString("FORMAT_%1_INDEX").        arg(fourcc)].  toUInt(); }
quint32     SScCamCapability::formatFlags       (const QString& fourcc) const   { return (*this)[QString("FORMAT_%1_FLAGS").        arg(fourcc)].  toUInt(); }
bool        SScCamCapability::formatCompressed  (const QString& fourcc) const   { return (*this)[QString("FORMAT_%1_COMPRESSED").   arg(fourcc)].  toBool(); }
bool        SScCamCapability::formatEmulated    (const QString& fourcc) const   { return (*this)[QString("FORMAT_%1_EMULATED").     arg(fourcc)].  toBool(); }
QString     SScCamCapability::formatDescriptor  (const QString& fourcc) const   { return (*this)[QString("FORMAT_%1_DESCRIPTOR").   arg(fourcc)].  toString(); }
QStringList SScCamCapability::frameSizes        (const QString& fourcc) const   { return (*this)[QString("FRAMESIZE_%1").arg(fourcc)].toString().split(","); }
QStringList SScCamCapability::formatList        () const                        { return (*this)["FORMAT_LIST"].toString().split(","); }

void SScCamCapability::dump() const
{
    const quint32 cap = caps(), dcap = devcaps();
    qWarning("Driver:        %s",  qPrintable( driver()));
    qWarning("Card:          %s",  qPrintable(   card()));
    qWarning("BusInfo:       %s",  qPrintable(busInfo()));
    qWarning("Version:       %u", version());
    qWarning("Caps:          %u", cap);
    qWarning("Devcaps:       %u", dcap);
    for (int i=0; i<32; ++i) if ( cap&(1<<i)) qWarning("Capability:    %s", qPrintable(cap2String(1<<i)));
    for (int i=0; i<32; ++i) if (dcap&(1<<i)) qWarning("DevCapability: %s", qPrintable(cap2String(1<<i)));
    foreach(const QString& fourcc, formatList())
    {
        qWarning("Format %u: %s (%s)",format2Index(fourcc), qPrintable(fourcc),qPrintable(formatDescriptor(fourcc)));
        qWarning("  Frame sizes  %s:", qPrintable(frameSizes(fourcc).join(",")));
    }
}

QList<quint32> SScCamCapability::devices()
{
    QSet<quint32> idxset;
    foreach(const QFileInfo& fi, QDir("/dev/").entryInfoList(QDir::System))
    {
        const QString fn = fi.fileName();
        if (fn.startsWith("video"))
        {
            bool ok = false;
            const quint32 nr = fn.right(fn.size()-5).toUInt(&ok);
            if (ok) idxset << nr;
        }
    }
    QList<quint32> retValue = idxset.toList();
    std::sort(retValue.begin(),retValue.end());
    return retValue;
}


QStringList SScCamCapability::deviceNames()
{
    QStringList retValue;
    foreach(const int idx, SScCamCapability::devices()) retValue << QString("/dev/video%1").arg(idx);
    return retValue;
}

SScCam::SScCam(QObject* parent) : QObject(parent), SScCamCapability(), m_fd(-1)
{

}
SScCam::SScCam(quint32 device, QObject *parent) : QObject(parent), SScCamCapability(device), m_fd(-1)
{}

SScCam::SScCam(const QString &device, QObject* parent) : QObject(parent), SScCamCapability(device), m_fd(-1)
{}

bool SScCam::streamOn()
{
    if (isOpen())
    {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl(m_fd, VIDIOC_STREAMON, &type) < 0) return false;
        return true;
    }
    return false;
}

bool SScCam::streamOff()
{
    if (isOpen())
    {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if(ioctl(m_fd, VIDIOC_STREAMON, &type) < 0) return false;
        return true;
    }
    return false;
}

bool SScCam::closeStream()
{
    if (isOpen())
    {
        streamOff();
    close(m_fd);
    m_fd=-1;
    return true;
    }
    return false;
}

bool SScCam::openStream(const QString &fourcc, quint32 w, quint32 h)
{
    qWarning("TRYIMNG TO OPEN %s %u x %u", qPrintable(fourcc),w,h);
    if (!isOpen() && allowed(fourcc,w,h))
    {

        qWarning("OPENING %s", qPrintable(fourcc2String(string2Fourcc(fourcc))));

        int fd;
        if ((fd = open(device().toUtf8().constData(), O_RDWR)) < 0) return false;


        struct v4l2_format format;
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.pixelformat = string2Fourcc(fourcc);
        format.fmt.pix.width = w;
        format.fmt.pix.height = h;

        if(ioctl(fd, VIDIOC_S_FMT, &format) < 0) { close(fd); return false; }

        struct v4l2_requestbuffers bufrequest;
        bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufrequest.memory = V4L2_MEMORY_MMAP;
        bufrequest.count = 1;

        if(ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0) { close(fd); return false; }

        struct v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));

        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = 0;

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

        m_buffers.append(buffer_start);


        memset(buffer_start, 0, bufferinfo.length);




        m_fd = fd;

        return true;
    }
    return false;
}

void SScCam::package(void* ptr)
{
   struct v4l2_buffer* buffer = static_cast<struct v4l2_buffer*>(ptr);

   qWarning("packaging buffer %u", buffer->index);
   QByteArray ba = QByteArray::fromRawData((const char*)m_buffers[buffer->index], buffer->length);
   qWarning("Byte array len %d", ba.size());
   QString fname = QString("/home/developer/%1.jpg").arg(QDateTime::currentDateTime().toString("hhmmsszzz"));
   QFile f(fname);
   qWarning("Trying to save %s", qPrintable(f.fileName()));

   if (f.open(QIODevice::WriteOnly))
   {
        f.write(ba);
        qWarning("Saved %s", qPrintable(f.fileName()));
        f.close();
   }

}

void SScCam::grabFrames(quint32 nr)
{
    if (isOpen())
    {
        struct v4l2_buffer bufferinfo;

        memset(&bufferinfo, 0, sizeof(bufferinfo));
        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = 0; /* Queueing buffer index 0. */

        bool success = true;
        // Put the buffer in the incoming queue.
        if(ioctl(m_fd, VIDIOC_QBUF, &bufferinfo) < 0) success = false;
        success = success && streamOn();

        if (success) while (nr>0)
        {
            --nr;
            if (ioctl(m_fd, VIDIOC_DQBUF, &bufferinfo) < 0) success = false;

            // Package the buffer
            if (success) package(&bufferinfo);

            bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            bufferinfo.memory = V4L2_MEMORY_MMAP;
            /* Set the index if using several buffers */

            // Queue the next one.
            if (ioctl(m_fd, VIDIOC_QBUF, &bufferinfo) < 0) success = false;
        }

        streamOff();
    }
}
