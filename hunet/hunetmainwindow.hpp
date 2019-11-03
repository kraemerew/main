#ifndef HUNETMAINWINDOW_HPP
#define HUNETMAINWINDOW_HPP

#include <QWidget>
#include <QTimer>
#include "watershedcontainer.hpp"
#include "contourcontainer.hpp"

class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
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
    void contourSlot    (SScContour);
    void loadSlot       ();
    void saveSlot       ();
    void tagSlot        (bool);
    void tryLoadSlot    (const QString&);

private:

    inline void recalc      () { if (!m_recalctimer.isActive()) m_recalctimer.start(10); }

    QTabWidget*             m_tab;
    HuNetImageDisplay*      m_procdisplay;
    SScContourContainer*    m_contourdisplay;
    QComboBox*              m_imgsel;
    QDoubleSpinBox*         m_mediansb;
    QDoubleSpinBox*         m_distsb;
    QDoubleSpinBox*         m_closesb;
    QCheckBox*              m_invcb;
    QCheckBox*              m_eqcb;

    QSpinBox*               m_binthr;

    QPushButton*            m_tagpos;
    QPushButton*            m_tagneg;
    QTimer                  m_recalctimer;
    SScWatershedContainer   m_wc;
    QString                 m_filename;
};

#endif // HUNETMAINWINDOW_HPP
