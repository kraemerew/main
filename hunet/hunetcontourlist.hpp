#ifndef HUNETCONTOURLIST_HPP
#define HUNETCONTOURLIST_HPP

#include <QListWidget>
#include "contour.hpp"

class HuNetContourList : public QListWidget
{
    Q_OBJECT

public:
    HuNetContourList(QWidget* parent = NULL);

    void set(const QList<SScContour>& cl);
signals:
    void selected(const SScContour&);

private slots:
    void selectedSlot(int);
private:
    QList<SScContour> m_cl;
};

#endif // HUNETCONTOURLIST_HPP
