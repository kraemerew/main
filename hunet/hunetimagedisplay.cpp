#include "hunetimagedisplay.hpp"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

HuNetImageDisplay::HuNetImageDisplay(QWidget* parent, const QSize& sz) : QLabel(parent), m_ca(NULL)
{
    setAcceptDrops(true);
    setFrameShape(QFrame::Box);
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setScaledContents(true);
    setFixedSize(sz);
}

HuNetImageDisplay::~HuNetImageDisplay() { if (m_ca) m_ca->deleteLater(); }

void HuNetImageDisplay::set(const QImage& im)
{
    setPixmap(QPixmap::fromImage(im));
}

QImage HuNetImageDisplay::get() const
{
    return pixmap()->toImage();
}

void HuNetImageDisplay::dragEnterEvent(QDragEnterEvent *event)
{
    if (allowDrops()) event->accept();
    else              event->ignore();
}
void HuNetImageDisplay::dragMoveEvent(QDragMoveEvent *event)
{
    if (allowDrops()) event->accept();
    else              event->ignore();
}
void HuNetImageDisplay::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (allowDrops()) event->accept();
    else              event->ignore();
}
void HuNetImageDisplay::dropEvent(QDropEvent* ev)
{
    if (allowDrops())
    {
        const QUrl url(ev->mimeData()->text());
        if (url.isLocalFile())
        {
            ev->accept();
            emit dropped(url.toLocalFile().trimmed());
        }
        else ev->ignore();
    }
    else ev->ignore();
}

QWidget* HuNetImageDisplay::controlArea()
{
    if (!m_ca)
    {
        m_ca = new (std::nothrow) QWidget;
        Q_CHECK_PTR(m_ca);
        m_ca->setLayout(new (std::nothrow) QVBoxLayout(m_ca));
        m_ca->hide();
    }
    return m_ca;
}

void HuNetImageDisplay::insertInto(QTabWidget* t, const QString& text)
{
    QWidget *w = new (std::nothrow) QWidget;
    Q_CHECK_PTR(w);
    QHBoxLayout* l = new (std::nothrow) QHBoxLayout;
    Q_CHECK_PTR(l);
    w->setLayout(l);
    t->addTab(w,text);
    l->setSpacing(0);
    l->addWidget(this);
    l->addWidget(controlArea());
    controlArea()->show();
}
void HuNetImageDisplay::insertStretch()
{
    QVBoxLayout* vl = dynamic_cast<QVBoxLayout*>(controlArea()->layout());
    Q_CHECK_PTR(vl);
    vl->addStretch();
}
void HuNetImageDisplay::insert(QWidget *w)
{
    Q_CHECK_PTR(w);
    QVBoxLayout* vl = dynamic_cast<QVBoxLayout*>(controlArea()->layout());
    Q_CHECK_PTR(vl);
    vl->addWidget(w);
}
