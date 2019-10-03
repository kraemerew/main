#ifndef CONTOURCONTAINER_HPP
#define CONTOURCONTAINER_HPP

#include "contour.hpp"
#include "hunetimagedisplay.hpp"

class QComboBox;
class HuNetContourList;

class SScContourContainer : public HuNetImageDisplay
{
    Q_OBJECT
public:
    explicit SScContourContainer(QWidget* parent = NULL, const QSize& sz = QSize(512,512));

    virtual ~SScContourContainer();

    void setContours(const QList<SScContour>& cl);
    QWidget* list() const;
    QList<SScContour> getContours() const;

private slots:
    void cbSlot         (int);
    void selectionSlot  (const SScContour&);

private:
    void updateDisplay();
    HuNetContourList*   m_contourlist;
    QComboBox*          m_cb;
};

#endif

