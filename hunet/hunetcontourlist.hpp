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
    SScContour currentContour() const;
    inline QString currentTag() const { return currentContour().tag(); }
    bool setTag(const QString& tag);
    void clear();

signals:
    void selected(const SScContour&);

private slots:
    void selectedSlot(QTableWidgetItem* oldit, QTableWidgetItem* newit);

private:
    void updateTag(const QString& md5);

    QMap<QTableWidgetItem*,QString> m_it2md5;
    QMap<QString,QTableWidgetItem*> m_md52tagit;
    SScContourSet                   m_cs;
};

#endif // HUNETCONTOURLIST_HPP
