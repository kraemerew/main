#include "imagedropper.hpp"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

HuNetImageDropper::HuNetImageDropper(QWidget* parent) : QLabel(parent)
{
    setAcceptDrops(true);
    setScaledContents(true);
    setMinimumSize(256,256);
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setText("Drop image here");
}
void HuNetImageDropper::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}
void HuNetImageDropper::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}
void HuNetImageDropper::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
void HuNetImageDropper::dropEvent(QDropEvent* ev)
{
    const QUrl url(ev->mimeData()->text());
    if (url.isLocalFile())
    {
        const QString filename = url.toLocalFile().trimmed();
        m_cc = SScCannyContainer(filename,21,false);
        if (m_cc.isValid())
        {
            qWarning(">>>>>>>>>>>>VALID");
            setText(QString());
            setPixmap(QPixmap::fromImage(m_cc.orig()));
        }
        /*auto cont = cc.contours(200,255);
        foreach (auto c, cont) if (c.size()>100)
        {
            qWarning("Contour size %d", (int)c.size());
            setPixmap(QPixmap::fromImage(c.draw(255)));
            break;
        }*/
    }
}
