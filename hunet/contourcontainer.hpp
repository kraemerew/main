#ifndef CONTOURCONTAINER_HPP
#define CONTOURCONTAINER_HPP

#include "contour.hpp"
#include "hunetimagedisplay.hpp"

class HuNetContourList;

class SScContourContainer : public HuNetImageDisplay
{
    Q_OBJECT
public:
    explicit SScContourContainer(QWidget* parent = NULL, const QSize& sz = QSize(512,512));

    virtual ~SScContourContainer();

    void setContours(const QList<SScContour>& cl);
    QObject* list() const;

public slots:
    void selectionSlot(const SScContour&);

private:
    HuNetContourList* m_contourlist;
};

#endif

