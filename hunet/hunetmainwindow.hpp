#ifndef HUNETMAINWINDOW_HPP
#define HUNETMAINWINDOW_HPP

#include <QWidget>
#include <QTimer>
#include "cannycontainer.hpp"
#include "contourcontainer.hpp"

class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class HuNetImageLoader;
class HuNetImageDisplay;
class QTabWidget;
class QPushButton;

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
    void loadSlot       ();
    void saveSlot       ();
    void tagSlot        (bool);

private:

    inline void recalc      () { if (!m_recalctimer.isActive()) m_recalctimer.start(10); }
    inline void recalcCanny () { if (!m_cannytimer. isActive()) m_cannytimer. start(10); }

    QTabWidget*             m_tab;
    HuNetImageLoader*       m_loader;
    HuNetImageDisplay*      m_procdisplay;
    HuNetImageDisplay*      m_cannydisplay;
    SScContourContainer*    m_contourdisplay;

    QDoubleSpinBox*     m_mediansb;
    QDoubleSpinBox*     m_bilcsigma;
    QDoubleSpinBox*     m_bilsigma;
    QCheckBox*          m_eqcb;
    QCheckBox*          m_clipcb;
    QCheckBox*          m_bilcb;
    QSpinBox*           m_cannymin;
    QSpinBox*           m_cannymax;
    QSpinBox*           m_bild;
    QPushButton*        m_tagpos;
    QPushButton*        m_tagneg;
    QTimer              m_recalctimer, m_cannytimer;
    SScCannyContainer   m_cc;
    QImage              m_orig;
};

#endif // HUNETMAINWINDOW_HPP
