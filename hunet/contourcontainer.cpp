#include "contourcontainer.hpp"
#include "hunetcontourlist.hpp"

SScContourContainer::SScContourContainer(QWidget* parent, const QSize& sz)
    : HuNetImageDisplay (parent,sz),
      m_contourlist     (new (std::nothrow) HuNetContourList)
{
    Q_CHECK_PTR(m_contourlist);
    insert(m_contourlist);

    bool ok;
    Q_UNUSED(ok);
    ok = connect(list(), SIGNAL(selected(const SScContour&)), this, SLOT(selectionSlot(const SScContour&)));
    Q_ASSERT(ok);
}

SScContourContainer::~SScContourContainer()
{
    m_contourlist->deleteLater();
}

void SScContourContainer::setContours(const QList<SScContour> &cl)
{
    m_contourlist->set(cl);
}

void SScContourContainer::selectionSlot(const SScContour& c)
{
    QImage im = c.draw(width(),2,Qt::gray);
    c.hull(2).draw(im,2,Qt::green);
    c.convexHull().draw(im,2,Qt::red,true);
    c.approxConvexHull(10).draw(im,5,Qt::blue,true);
    set(im);
}

QObject* SScContourContainer::list() const { return m_contourlist; }
