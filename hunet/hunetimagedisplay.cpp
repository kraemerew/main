#include "hunetimagedisplay.hpp"

HuNetImageDisplay::HuNetImageDisplay(QWidget* parent) : QLabel(parent)
{
    setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setScaledContents(true);
    setFixedSize(256,256);
}

void HuNetImageDisplay::set(const QImage& im)
{
    setPixmap(QPixmap::fromImage(im));
}
