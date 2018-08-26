#include "camwidget.hpp"
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include <QFontMetrics>
#include "image/image.hpp"
#include "image/matrix.hpp"
#include "cam/cam.hpp"
#include "cam/camcapability.hpp"

class SScCamWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    SScCamWidgetPrivate(SScCamWidget* parent)
        : QObject(parent),
          m_dev  (new (std::nothrow) QComboBox  (parent)),
          m_res  (new (std::nothrow) QComboBox  (parent)),
          m_layer(new (std::nothrow) QComboBox  (parent)),
          m_sa   (new (std::nothrow) QScrollArea(parent)),
          m_frame(new (std::nothrow) QLabel     (parent)),
          m_fps  (new (std::nothrow) QLabel     (parent)),
          m_cam  (NULL),
          m_fctr (0)

    {
        Q_CHECK_PTR(m_dev);
        Q_CHECK_PTR(m_res);
        Q_CHECK_PTR(m_sa);        
        Q_CHECK_PTR(m_layer);
        Q_CHECK_PTR(m_frame);
        Q_CHECK_PTR(m_fps);

        m_layer->addItem("Color",   0);
        m_layer->addItem("Red",     1);
        m_layer->addItem("Green",   2);
        m_layer->addItem("Blue",    3);

        QVBoxLayout* vl = new (std::nothrow) QVBoxLayout (); Q_CHECK_PTR(vl);
        QHBoxLayout* hl = new (std::nothrow) QHBoxLayout (); Q_CHECK_PTR(hl);
        parent->setLayout(vl);
        vl->addItem(hl);
        vl->addWidget(m_sa);

        hl->addWidget(m_dev);
        hl->addWidget(m_res);
        hl->addWidget(m_layer);
        hl->addStretch();
        hl->addWidget(m_fps);

        m_sa->setWidget(m_frame);

        m_frame->setScaledContents(true);
        parent->setMinimumSize(QSize(320,240));

        init();

        const int w = QFontMetrics(parent->font()).width("X");
        m_dev->setMinimumWidth(w*16);
        m_res->setMinimumWidth(w*16);
        bool ok;
        Q_UNUSED(ok);
        ok = connect(m_dev,SIGNAL(activated(int)),this,SLOT(devSlot(int)));
        ok = connect(m_res,SIGNAL(activated(int)),this,SLOT(resSlot(int)));

        if (m_dev->count()==2) m_dev->setCurrentIndex(1);
        devSlot(m_dev->currentIndex());
    }
    virtual ~SScCamWidgetPrivate()
    {
        stopCamera();
    }

private slots:
    void frameSlot(const QImage& im)
    {       
        m_frame->setPixmap(QPixmap::fromImage(processedFrame(im)));
        ++m_fctr;
        if (m_ftimer.elapsed()>1000)
        {
            const double fps = (double)(1000*m_fctr)/m_ftimer.elapsed();
            QString s;
            s.sprintf("%.1lf Fps", fps);
            m_fps -> setText(s);
            m_fctr = 0;
            m_ftimer.restart();
        }
    }

    void devSlot(int idx)
    {
        Q_UNUSED(idx);
        stopCamera();
        initRes();
        if (m_res->count()!=0) resSlot(m_res->currentIndex());
    }
    void resSlot(int idx)
    {
        Q_UNUSED(idx);
        startCamera();
    }
    void retryStartSlot()
    {
        startCamera();
    }

private:
    void stopCamera()
    {
        if (m_cam) delete m_cam;
        m_cam = NULL;
    }
    QImage processedFrame(const QImage& im) const
    {
        const int l = m_layer->itemData(m_layer->currentIndex()).toInt();
        if (l>0)
        {
            SScImage img(im), img2;
            switch(l)
            {
                case 1: img2 = SScImage(img.red  ());  break; //img2.save("/home/developer/test_red.jpg"); break;
                case 2: img2 = SScImage(img.green());  break; //img2.save("/home/developer/test_green.jpg"); break;
                case 3: img2 = SScImage(img.blue ());  break;img2.save("/home/developer/test_blue.jpg"); break;
            }
            qWarning("SIZE %d %d LAYER %d", img2.width(),img2.height(),l);
            return img2;
        }
        return im;
    }
    void startCamera()
    {
        stopCamera();
        auto res = currentRes();
        if ((res.first>0) && (res.second>0))
        {
            m_frame->setFixedSize(res.first,res.second);
            m_cam = new (std::nothrow) SScCam(currentDevice(),currentFourcc(),res.first,res.second);
            Q_CHECK_PTR(m_cam);
            if (m_cam->isOpen())
            {
                m_ftimer.start();
                m_fctr=0;
                connect(m_cam, SIGNAL(frame(const QImage&)), this, SLOT(frameSlot(const QImage&)));
                m_cam->streamOn();
            }
            else
            {
                qWarning("Could not open");
            }
        }
    }

    void initRes()
    {
        m_res->blockSignals(true);
        m_res->clear();
        m_res->setEnabled(m_dev->currentIndex()!=0);
        SScCamCapability cap(m_dev->currentText());
        foreach(const QString& s, cap.formatList()) if (s=="MJPG")
        {
            auto res = cap.resolutions("MJPG");
            foreach(auto p, res) m_res->addItem(QString("%1 x %2 MJPG").arg(p.first).arg(p.second));
        }
        m_res->blockSignals(false);
    }

    void init()
    {
        const QStringList sl = QStringList() << "Device: none" << SScCamCapability::deviceNames();
        foreach(const QString& s, sl) m_dev->addItem(s);
    }

    QPair<quint32,quint32> currentRes() const
    {
        QStringList sl = m_res->currentText().split(" ");
        if (sl.size()>=4)
        {
            const QString s0 = sl.takeFirst(), s1 = sl.takeFirst(), s2 = sl.takeFirst();
            bool ok1, ok2, ok3 = (s1.simplified().toUpper()=="X");
            const quint32 w = s0.toInt(&ok1), h = s2.toInt(&ok2);
            if (ok1 && ok2 && ok3) return QPair<quint32,quint32>(w,h);
        }
        return QPair<quint32,quint32>(0,0);
    }
    QString currentFourcc() const
    {
        QStringList sl = m_res->currentText().split(" ");
        if (sl.size()>=4)
        {
            sl.pop_front();
            sl.pop_front();
            sl.pop_front();
            return sl.first();
        }
        return QString();
    }
    QString currentDevice() const
    {
        if (m_dev->currentIndex()==0) return QString();
        return m_dev->currentText();
    }

private:
    QComboBox*      m_dev;
    QComboBox*      m_res;
    QComboBox*      m_layer;
    QScrollArea*    m_sa;
    QLabel*         m_frame;
    QLabel*         m_fps;
    SScCam*         m_cam;
    quint32         m_fctr;
    QElapsedTimer   m_ftimer;
};

#include "camwidget.moc"

SScCamWidget::SScCamWidget(QWidget* parent) : QWidget(parent), d_ptr(new (std::nothrow) SScCamWidgetPrivate(this))
{}
