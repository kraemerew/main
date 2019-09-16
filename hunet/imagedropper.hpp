#ifndef IMAGEDROPPER_HPP
#define IMAGEDROPPER_HPP

#include <QLabel>
#include "cannycontainer.hpp"
class HuNetImageDropper : public QLabel
{
public:
    explicit HuNetImageDropper(QWidget* parent = NULL);

private:
    SScCannyContainer m_cc;
    void dragEnterEvent (QDragEnterEvent *event);
    void dragMoveEvent  (QDragMoveEvent *event);
    void dragLeaveEvent (QDragLeaveEvent *event);
    void dropEvent      (QDropEvent* ev);
};

#endif // IMAGEDROPPER_HPP
