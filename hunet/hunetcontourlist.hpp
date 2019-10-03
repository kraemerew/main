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
    inline QList<SScContour> get() const { return m_cs.toList(); }
    void clear();
    SScContour currentContour() const;
    SScContour contourForLine(int) const;

    inline QString currentTag() const { return currentContour().tag(); }

    bool setTag(const QString& tag);

signals:
    void selected(const SScContour&);

private slots:
    void selectedSlot(QTableWidgetItem* oldit, QTableWidgetItem* newit);

private:
    QMap<QTableWidgetItem*, int>    m_it2line;
    QMap<int,QString>               m_line2md5;
    SScContourSet                   m_cs;
};

#endif // HUNETCONTOURLIST_HPP
