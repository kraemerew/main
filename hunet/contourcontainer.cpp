#include "contourcontainer.hpp"
#include "hunetcontourlist.hpp"
#include <QComboBox>

SScContourContainer::SScContourContainer(QWidget* parent, const QSize& sz)
    : HuNetImageDisplay (parent,sz),
      m_contourlist     (new (std::nothrow) HuNetContourList),
      m_cb              (new (std::nothrow) QComboBox)
{
    Q_CHECK_PTR(m_contourlist);
    Q_CHECK_PTR(m_cb);
    insert(m_cb);

    m_contourlist->setEnabled(false);
    m_cb->addItems(SScContour::contourLabels());
    m_contourlist->setFrameStyle(QFrame::Box);
    bool ok;
    Q_UNUSED(ok);
    ok = connect(list(), SIGNAL(selected(const SScContour&)), this, SLOT(selectionSlot(const SScContour&)));
    Q_ASSERT(ok);
    ok = connect(m_cb, SIGNAL(activated(int)), this, SLOT(cbSlot(int)));
    Q_ASSERT(ok);
}

SScContourContainer::~SScContourContainer()
{
    m_contourlist   ->deleteLater();
    m_cb            ->deleteLater();
}

void SScContourContainer::cbSlot(int)
{
    updateDisplay();
}
void SScContourContainer::setContours(const QList<SScContour> &cl)
{
    m_contourlist->set(cl);
    m_contourlist->setEnabled(!cl.isEmpty());
}

QList<SScContour> SScContourContainer::getContours() const
{
    return m_contourlist->get();
}

void SScContourContainer::selectionSlot(const SScContour&)
{
    updateDisplay();
}

QWidget* SScContourContainer::list() const { return m_contourlist; }

void SScContourContainer::updateDisplay()
{
    auto c = m_contourlist->currentContour();
    if (c.isEmpty()) return;
    const auto id = m_cb->currentText();
    QImage im = c.draw(width(),2,Qt::gray);
    c = c.contour(id);
    if (!c.isEmpty()) c.draw(im,5,Qt::red,true);
    set(im);
}
