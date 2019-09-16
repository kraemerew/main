#include "hunetcontourlist.hpp"

HuNetContourList::HuNetContourList(QWidget* parent) : QListWidget(parent)
{
    qRegisterMetaType<SScContour>("SScContour");
    setIconSize(QSize(32,32));
    const bool ok = connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(selectedSlot(int)));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}


void HuNetContourList::set(const QList<SScContour>& cl)
{
    blockSignals(true);
    clear();
    m_cl = cl;
    foreach(auto c, cl)
    {
        const QPixmap pm = QPixmap::fromImage(c.draw(32));
        QListWidgetItem * it = new(std::nothrow) QListWidgetItem(this);
        Q_CHECK_PTR(it);
        it->setIcon(QIcon(pm));
        it->setText(c.label());
        addItem(it);
    }
    if (cl.isEmpty()) hide(); else show();
    blockSignals(false);
    setCurrentRow(0);
}

void HuNetContourList::selectedSlot(int row)
{
    SScContour c;
    if ((row>=0) && (row<m_cl.size()))
        c = m_cl[row];
    emit selected (c);
}
