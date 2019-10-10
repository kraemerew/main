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
    m_it2md5.     clear();
    m_md52tagit.  clear();
}

void HuNetContourList::set(const QList<SScContour>& cl)
{
    clear();
    m_cs = SScContourSet(cl);
    blockSignals(true);

    verticalHeader()->hide();

    const auto labels = SScContour::featureLabels();
    setColumnCount(labels.size()+2);
    setHorizontalHeaderLabels(QStringList() << "" << "Tag" <<  labels);
    setRowCount(m_cs.size());
    horizontalHeaderItem(0)->setFlags(Qt::NoItemFlags);
    horizontalHeaderItem(1)->setFlags(Qt::NoItemFlags);

    int row = -1;
    foreach(const QString& md5, m_cs.keys())
    {
        SScContour c = m_cs[md5];
        int col = -1;
        ++row;

        const QPixmap pm = QPixmap::fromImage(c.draw(32));
        QTableWidgetItem* it = new (std::nothrow) QTableWidgetItem;
        Q_CHECK_PTR(it);
        m_it2md5[it]=md5;

        it->setIcon(QIcon(pm));
        setItem(row,++col,it);
        it = new (std::nothrow) QTableWidgetItem;
        Q_CHECK_PTR(it);
        m_it2md5[it]=md5;
        m_md52tagit[md5]=it;
        it->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        it->setText(c.tag());
        setItem(row,++col,it);

        foreach(const QString& s, c.featureValues())
        {
            it = new (std::nothrow) QTableWidgetItem;
            Q_CHECK_PTR(it);
            m_it2md5[it]=md5;
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
    if (newit && m_it2md5.contains(newit))
    {
        const QString md5 = m_it2md5[newit];
        if (m_cs.contains(md5)) c = m_cs[md5];
    }
    emit selected (c);
}


SScContour HuNetContourList::currentContour() const
{
    auto it = currentItem();
    SScContour c;
     if (it && m_it2md5.contains(it))
    {
        const QString md5 = m_it2md5[it];
        if (m_cs.contains(md5)) c = m_cs[md5];
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
    if (m_md52tagit.contains(md5))
    {
        const auto tag = m_cs[md5].tag();
        m_md52tagit[md5]->setText(tag);
    }
}

