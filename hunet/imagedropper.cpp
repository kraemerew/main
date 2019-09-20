#include "imagedropper.hpp"

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "imageconverter.h"

HuNetImageDropper::HuNetImageDropper(QWidget* parent) : HuNetImageDisplay(parent), m_label(new (std::nothrow) QLabel)
{
    setText("Drop image here");
    Q_CHECK_PTR(m_label);
    insert(m_label);
    m_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
HuNetImageDropper::~HuNetImageDropper() { delete m_label; }

bool HuNetImageDropper::dropped(const QString& filename)
{
    if (filename!=m_filename)
    {
        cv::Mat im = cv::imread(filename.toUtf8().constData(),cv::IMREAD_GRAYSCALE);
        if ((im.cols>0) && (im.rows>0))
        {
            m_filename = filename;
            setText(QString());
            set(SSnImageConverter::image(im));
            m_label->setText(QString("Image %1x%2").arg(im.cols).arg(im.rows));
            emit loaded(filename);
            return true;
        }
    }
    return false;
}
