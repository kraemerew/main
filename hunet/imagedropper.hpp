#ifndef IMAGEDROPPER_HPP
#define IMAGEDROPPER_HPP

#include "hunetimagedisplay.hpp"
class HuNetImageDropper : public HuNetImageDisplay
{
    Q_OBJECT

public:
    explicit HuNetImageDropper(QWidget* parent = NULL);
    inline QString filename() const { return m_filename; }

signals:
    void loaded(const QString&);

private:
    QString m_filename;

    void dragEnterEvent (QDragEnterEvent *event);
    void dragMoveEvent  (QDragMoveEvent  *event);
    void dragLeaveEvent (QDragLeaveEvent *event);
    void dropEvent      (QDropEvent* ev);
};

#endif // IMAGEDROPPER_HPP
