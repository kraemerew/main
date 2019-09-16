#include "hunetmainwindow.hpp"
#include "imagedropper.hpp"
#include "hunetcannydisplay.hpp"
#include "hunetcontourlist.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

HuNetMainWindow::HuNetMainWindow()
    : QWidget       (0),
      m_dropper     (new (std::nothrow) HuNetImageDropper(this)),
      m_cannydisplay(new (std::nothrow) HuNetCannyDisplay(this)),
      m_contourlist (new (std::nothrow) HuNetContourList (this)),
      m_mediansb    (new (std::nothrow) QDoubleSpinBox   (this)),
      m_eqcb        (new (std::nothrow) QCheckBox        (this)),
      m_cannymin    (new (std::nothrow) QSpinBox         (this)),
      m_cannymax    (new (std::nothrow) QSpinBox         (this))
{
    QHBoxLayout* hl = new (std::nothrow) QHBoxLayout;
    QVBoxLayout* vl = new (std::nothrow) QVBoxLayout;
    Q_CHECK_PTR(hl);
    Q_CHECK_PTR(vl);
    Q_CHECK_PTR(m_dropper);
    Q_CHECK_PTR(m_cannydisplay);
    Q_CHECK_PTR(m_contourlist);
    Q_CHECK_PTR(m_mediansb);
    Q_CHECK_PTR(m_eqcb);
    Q_CHECK_PTR(m_cannymin);
    Q_CHECK_PTR(m_cannymax);

    vl->addWidget(m_mediansb);
    vl->addWidget(m_eqcb);
    vl->addWidget(m_cannymin);
    vl->addWidget(m_cannymax);
    vl->addStretch();

    m_mediansb  ->setPrefix("Median: ");
    m_mediansb  ->setSuffix(("%"));
    m_eqcb      ->setText("Histogram Eq.");
    m_cannymin  ->setPrefix("Canny lower ");
    m_cannymax  ->setPrefix("Canny upper ");

    m_mediansb->setRange(0,99);
    m_cannymin->setRange(0,254);
    m_cannymax->setRange(1,255);

    setLayout(hl);
    hl->addWidget(m_dropper);
    hl->addItem(vl);
    hl->addWidget(m_cannydisplay);
    hl->addWidget(m_contourlist);
    m_cannydisplay  ->hide();
    m_contourlist   ->hide();

    m_cannymin  ->setValue  (200);
    m_cannymax  ->setValue  (250);
    m_eqcb      ->setChecked(true);
    m_mediansb  ->setValue(1.0);

    bool ok;
    Q_UNUSED(ok);
    ok = connect(m_mediansb,    SIGNAL(valueChanged(double)),   this,   SLOT(doubleSlot(double)));  Q_ASSERT(ok);
    ok = connect(m_cannymin,    SIGNAL(valueChanged(int)),      this,   SLOT(intSlot(int)));        Q_ASSERT(ok);
    ok = connect(m_cannymax,    SIGNAL(valueChanged(int)),      this,   SLOT(intSlot(int)));        Q_ASSERT(ok);
    ok = connect(m_eqcb,        SIGNAL(toggled(bool)),          this,   SLOT(boolSlot(bool)));      Q_ASSERT(ok);
}

HuNetMainWindow::~HuNetMainWindow()
{
}


void HuNetMainWindow::doubleSlot(double)
{}

void HuNetMainWindow::intSlot(int v)
{
    if (sender()==m_cannymin)
    {
        if (m_cannymax->value()<=v)
        {
            m_cannymax->blockSignals(true);
            m_cannymax->setValue(v+1);
            m_cannymax->blockSignals(false);
        }
    }
    else if (sender()==m_cannymax)
    {
        if (m_cannymin->value()>=v)
        {
            m_cannymin->blockSignals(true);
            m_cannymin->setValue(v-1);
            m_cannymin->blockSignals(false);
        }
    }
}
void HuNetMainWindow::boolSlot(bool)
{

}
