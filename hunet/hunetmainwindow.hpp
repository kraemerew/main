#ifndef HUNETMAINWINDOW_HPP
#define HUNETMAINWINDOW_HPP

#include <QWidget>

class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class HuNetImageDropper;
class HuNetCannyDisplay;
class HuNetContourList;

class HuNetMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HuNetMainWindow();
    virtual ~HuNetMainWindow();

private slots:
    void doubleSlot (double);
    void intSlot    (int);
    void boolSlot   (bool);

private:
    HuNetImageDropper*  m_dropper;
    HuNetCannyDisplay*  m_cannydisplay;
    HuNetContourList*   m_contourlist;
    QDoubleSpinBox*     m_mediansb;
    QCheckBox*          m_eqcb;
    QSpinBox*           m_cannymin;
    QSpinBox*           m_cannymax;
};

#endif // HUNETMAINWINDOW_HPP
