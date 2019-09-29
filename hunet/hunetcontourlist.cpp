#include "hunetcontourlist.hpp"
#include <QHeaderView>
#include <QTableWidgetItem>

HuNetContourList::HuNetContourList(QWidget* parent) : QTableWidget(parent)
{
    qRegisterMetaType<SScContour>("SScContour");
    setIconSize(QSize(32,32));
    setSortingEnabled(true);
    const bool ok = connect(this, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
                            this, SLOT(selectedSlot(QTableWidgetItem*,QTableWidgetItem*)));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void HuNetContourList::clear()
{
    QTableWidget::clear();
    m_contours.   clear();
    m_it2idx.     clear();
}

void HuNetContourList::set(const QList<SScContour>& cl)
{
    blockSignals(true);
    clear();

    verticalHeader()->hide();

    auto labels = SScContour::featureLabels();
    setColumnCount(labels.size()+1);
    setHorizontalHeaderLabels(QStringList() << "" << labels);
    setRowCount(cl.size());

    m_contours = cl;
    int row = -1;
    foreach(auto c, cl)
    {
        int col = -1;
        const QPixmap pm = QPixmap::fromImage(c.draw(32));
        QTableWidgetItem* it = new (std::nothrow) QTableWidgetItem;
        Q_CHECK_PTR(it);
        m_it2idx[it]=++row;
        it->setIcon(QIcon(pm));
        setItem(row,++col,it);
        foreach(const QString& s, c.featureValues())
        {
            it = new (std::nothrow) QTableWidgetItem;
            Q_CHECK_PTR(it);
            m_it2idx[it]=row;
            it->setText(s);
            setItem(row,++col,it);
        }
    }
    if (cl.isEmpty()) hide(); else show();
    blockSignals(false);
    if (!m_contours.isEmpty()) setCurrentCell(0,0);
}

void HuNetContourList::selectedSlot(QTableWidgetItem *newit, QTableWidgetItem *)
{
    SScContour c;
    if (newit && m_it2idx.contains(newit)) c = m_contours[m_it2idx[newit]];
    emit selected (c);
}

SScContour HuNetContourList::currentContour() const
{
    auto it = currentItem();
    SScContour c;
    if (it && m_it2idx.contains(it)) c = m_contours[m_it2idx[it]];
    return c;
}
