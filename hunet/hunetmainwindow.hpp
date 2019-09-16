#ifndef HUNETMAINWINDOW_HPP
#define HUNETMAINWINDOW_HPP

#include <QWidget>
#include <QTimer>
#include "cannycontainer.hpp"

class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class HuNetImageDropper;
class HuNetImageDisplay;
class HuNetContourList;

class HuNetMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HuNetMainWindow();
    virtual ~HuNetMainWindow();

private slots:
    void doubleSlot     (double);
    void intSlot        (int);
    void boolSlot       (bool);
    void recalcSlot     ();
    void recalcCannySlot();
    void stringSlot     (const QString&);
    void contourSlot    (SScContour);
private:

    inline void recalc      () { if (!m_recalctimer.isActive()) m_recalctimer.start(100); }
    inline void recalcCanny () { if (!m_cannytimer. isActive()) m_cannytimer. start(100); }

    HuNetImageDropper*  m_dropper;
    HuNetImageDisplay*  m_procdisplay;
    HuNetImageDisplay*  m_cannydisplay;
    HuNetContourList*   m_contourlist;
    HuNetImageDisplay*  m_contourdisplay;

    QWidget*            m_w1;
    QWidget*            m_w2;
    QDoubleSpinBox*     m_mediansb;
    QCheckBox*          m_eqcb;
    QSpinBox*           m_cannymin;
    QSpinBox*           m_cannymax;

    QTimer              m_recalctimer, m_cannytimer;
    SScCannyContainer   m_cc;
};

#endif // HUNETMAINWINDOW_HPP
