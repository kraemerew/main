#include "hunetcontourlist.hpp"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFile>

HuNetContourList::HuNetContourList(QWidget* parent) : QTableWidget(parent)
{
    qRegisterMetaType<SScContour>("SScContour");
    setIconSize(QSize(32,32));
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    const bool ok = connect(this, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
                            this, SLOT(selectedSlot(QTableWidgetItem*,QTableWidgetItem*)));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void HuNetContourList::clear()
{
    QTableWidget::clear();
    m_cs.         clear();
    m_it2line.    clear();
    m_line2md5.   clear();
}

void HuNetContourList::set(const QList<SScContour>& cl)
{
    clear();
    m_cs = SScContourSet(cl);
    blockSignals(true);

    verticalHeader()->hide();

    auto labels = SScContour::featureLabels();
    setColumnCount(labels.size()+2);
    setHorizontalHeaderLabels(QStringList() << "" << "Tag" <<  labels);
    setRowCount(m_cs.size());

    int row = -1;
    foreach(const QString& md5, m_cs.keys())
    {
        SScContour c = m_cs[md5];
        int col = -1;
        ++row;
        m_line2md5[row]=md5;

        const QPixmap pm = QPixmap::fromImage(c.draw(32));
        QTableWidgetItem* it = new (std::nothrow) QTableWidgetItem;
        Q_CHECK_PTR(it);
        m_it2line[it]=row;

        it->setIcon(QIcon(pm));
        setItem(row,++col,it);
        it = new (std::nothrow) QTableWidgetItem;
        Q_CHECK_PTR(it);
        m_it2line[it]=row;

        it->setText(c.tag());
        setItem(row,++col,it);

        foreach(const QString& s, c.featureValues())
        {
            it = new (std::nothrow) QTableWidgetItem;
            Q_CHECK_PTR(it);
            m_it2line[it]=row;
            it->setText(s);
            setItem(row,++col,it);
        }
    }
    blockSignals(false);
    if (m_cs.isEmpty()) hide(); else show();
    if (!m_cs.isEmpty()) setCurrentCell(0,0);
}

void HuNetContourList::selectedSlot(QTableWidgetItem *newit, QTableWidgetItem *)
{
    SScContour c;
    if (newit && m_it2line.contains(newit))
    {
        const int line = m_it2line[newit];
        c = contourForLine(line);
    }
    emit selected (c);
}

SScContour HuNetContourList::contourForLine(int line) const
{
    SScContour c;
    if (m_line2md5.contains(line))
    {
        c = m_cs[m_line2md5[line]];
    }
    return c;
}

SScContour HuNetContourList::currentContour() const
{
    auto it = currentItem();
    SScContour c;
    if (it && m_it2line.contains(it))
    {
        c = contourForLine(m_it2line[it]);
    }
    return c;
}

bool HuNetContourList::setTag(const QString &tag)
{
    auto c = currentContour();
    if (!c.isEmpty())
    {
        if (m_cs.tag(c.md5(),tag)) updateTag(c.md5());
    }
    return false;
}

void HuNetContourList::updateTag(const QString& md5)
{
    const auto tag = m_cs[md5].tag();
    const auto line = md52line(md5);
    if ((line>=0) && item(line,1)) item(line,1)->setText(tag);
}

int HuNetContourList::md52line(const QString& md5) const
{
    foreach (auto i, m_line2md5.keys()) if (m_line2md5[i]==md5) return i;
    return -1;
}
