#ifndef HUNETCONTOURLIST_HPP
#define HUNETCONTOURLIST_HPP

#include <QTableWidget>
#include "contour.hpp"

class HuNetContourList : public QTableWidget
{
    Q_OBJECT

public:
    HuNetContourList(QWidget* parent = NULL);

    void set(const QList<SScContour>& cl);
    void clear();
    SScContour currentContour() const;

signals:
    void selected(const SScContour&);

private slots:
    void selectedSlot(QTableWidgetItem* oldit, QTableWidgetItem* newit);

private:
    QMap<QTableWidgetItem*, int> m_it2idx;
    QList<SScContour> m_contours;
};

#endif // HUNETCONTOURLIST_HPP
