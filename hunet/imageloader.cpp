#include "imageloader.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "imageconverter.h"
#include "watershed.hpp"
HuNetImageLoader::HuNetImageLoader(QWidget* parent)
    : HuNetImageDisplay(parent),
      m_label(new (std::nothrow) QLabel)
{
    setText("Drop image here");
    Q_CHECK_PTR(m_label);
    insert(m_label);
    m_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}
HuNetImageLoader::~HuNetImageLoader() { delete m_label; }

bool HuNetImageLoader::tryLoad(const QString& filename)
{
    if (filename!=m_filename)
    {
        cv::Mat im = cv::imread(filename.toUtf8().constData(),cv::IMREAD_COLOR);
        if ((im.cols>0) && (im.rows>0))
        {
            cv::Mat trg;

            SSnWatershed::execute(im,SSnWatershed::Pars(0.0,1,40,true,.4));

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

