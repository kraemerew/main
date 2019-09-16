#include "imagedropper.hpp"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "imageconverter.h"

HuNetImageDropper::HuNetImageDropper(QWidget* parent) : HuNetImageDisplay(parent)
{
    setAcceptDrops(true);
    setScaledContents(true);
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
        if (filename!=m_filename)
        {
            cv::Mat im = cv::imread(filename.toUtf8().constData(),cv::IMREAD_GRAYSCALE);
            if ((im.cols>0) && (im.rows>0))
            {
                m_filename = filename;
                setText(QString());
                set(SSnImageConverter::image(im));
                emit loaded(filename);
            }
        }
    }
}
