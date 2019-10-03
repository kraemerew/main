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
    inline QList<SScContour> get() const { return m_contours; }
    void clear();
    SScContour currentContour() const;

    bool save(const QString& filename);
    bool load(const QString& filename);

signals:
    void selected(const SScContour&);

private slots:
    void selectedSlot(QTableWidgetItem* oldit, QTableWidgetItem* newit);

private:
    QMap<QTableWidgetItem*, int>    m_it2idx;
    QList<SScContour>               m_contours;
};

#endif // HUNETCONTOURLIST_HPP
