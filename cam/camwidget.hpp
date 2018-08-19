#ifndef CAMWIDGET_HPP
#define CAMWIDGET_HPP

#include <QWidget>

class SScCamWidgetPrivate;

class SScCamWidget : public QWidget
{
    Q_DISABLE_COPY(SScCamWidget)

public:
    SScCamWidget(QWidget* parent = 0);

private:
    SScCamWidgetPrivate* d_ptr;
};

#endif // CAMWIDGET_HPP
