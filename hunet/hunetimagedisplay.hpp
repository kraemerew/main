#ifndef HUNETIMAGEDISPLAY_HPP
#define HUNETIMAGEDISPLAY_HPP

#include <QLabel>
#include <QImage>
class QTabWidget;

class HuNetImageDisplay : public QLabel
{
    Q_OBJECT

public:
    explicit HuNetImageDisplay(QWidget* parent = NULL, const QSize& sz = QSize(512,512));
    virtual ~HuNetImageDisplay();

    void set(const QImage&);
    QImage get() const;

    virtual bool allowDrops() const { return true; }

    void insertInto(QTabWidget*,const QString&);
    void insert(QWidget*);
    void insertStretch();

signals:
    void dropped(const QString&);

protected:
    void dragEnterEvent (QDragEnterEvent*);
    void dragMoveEvent  (QDragMoveEvent*);
    void dragLeaveEvent (QDragLeaveEvent*);
    void dropEvent      (QDropEvent*);

    QWidget* controlArea();
    QWidget* m_ca;
};

#endif // HUNETIMAGEDISPLAY_HPP
