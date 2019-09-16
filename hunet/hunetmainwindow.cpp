#include "hunetmainwindow.hpp"
#include "imagedropper.hpp"
#include "hunetimagedisplay.hpp"
#include "hunetcontourlist.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

HuNetMainWindow::HuNetMainWindow()
    : QWidget       (0),
      m_dropper     (new (std::nothrow) HuNetImageDropper(this)),
      m_procdisplay (new (std::nothrow) HuNetImageDisplay(this)),
      m_cannydisplay(new (std::nothrow) HuNetImageDisplay(this)),
      m_contourlist     (new (std::nothrow) HuNetContourList (this)),
      m_contourdisplay  (new (std::nothrow) HuNetImageDisplay(this)),
      m_w1          (new (std::nothrow) QWidget          (this)),
      m_w2          (new (std::nothrow) QWidget          (this)),
      m_mediansb    (new (std::nothrow) QDoubleSpinBox   (this)),
      m_eqcb        (new (std::nothrow) QCheckBox        (this)),
      m_cannymin    (new (std::nothrow) QSpinBox         (this)),
      m_cannymax    (new (std::nothrow) QSpinBox         (this))
{    
    QHBoxLayout* hl  = new (std::nothrow) QHBoxLayout;
    QVBoxLayout* vl1 = new (std::nothrow) QVBoxLayout;
    QVBoxLayout* vl2 = new (std::nothrow) QVBoxLayout;

    Q_CHECK_PTR(hl);
    Q_CHECK_PTR(vl1);
    Q_CHECK_PTR(vl2);
    Q_CHECK_PTR(m_dropper);
    Q_CHECK_PTR(m_procdisplay);
    Q_CHECK_PTR(m_cannydisplay);
    Q_CHECK_PTR(m_w1);
    Q_CHECK_PTR(m_w2);
    Q_CHECK_PTR(m_contourlist);
    Q_CHECK_PTR(m_contourdisplay);
    Q_CHECK_PTR(m_mediansb);
    Q_CHECK_PTR(m_eqcb);
    Q_CHECK_PTR(m_cannymin);
    Q_CHECK_PTR(m_cannymax);

    vl1->addWidget(m_mediansb);
    vl1->addWidget(m_eqcb);
    vl1->addStretch();

    vl2->addWidget(m_cannymin);
    vl2->addWidget(m_cannymax);
    vl2->addStretch();

    m_mediansb  ->setPrefix("Median: ");
    m_mediansb  ->setSuffix(("%"));
    m_eqcb      ->setText("Histogram Eq.");
    m_cannymin  ->setPrefix("Canny lower ");
    m_cannymax  ->setPrefix("Canny upper ");

    m_mediansb->setRange(0,99);
    m_cannymin->setRange(0,254);
    m_cannymax->setRange(1,255);
    m_w1->setLayout(vl1);
    m_w2->setLayout(vl2);

    setLayout(hl);
    hl->addWidget(m_dropper);
    hl->addWidget(m_w1);
    hl->addWidget(m_procdisplay);
    hl->addWidget(m_w2);
    hl->addWidget(m_cannydisplay);
    hl->addWidget(m_contourlist);
    hl->addWidget(m_contourdisplay);

    m_procdisplay   ->hide();
    m_cannydisplay  ->hide();
    m_contourlist   ->hide();
    m_w1            ->hide();
    m_w2            ->hide();

    m_cannymin  ->setValue  (200);
    m_cannymax  ->setValue  (250);
    m_eqcb      ->setChecked(true);
    m_mediansb  ->setValue(1.0);

    bool ok;
    Q_UNUSED(ok);
    ok = connect(m_dropper,     SIGNAL(loaded(const QString&)),     this,   SLOT(stringSlot(const QString&)));      Q_ASSERT(ok);
    ok = connect(m_mediansb,    SIGNAL(valueChanged(double)),       this,   SLOT(doubleSlot(double)));              Q_ASSERT(ok);
    ok = connect(m_cannymin,    SIGNAL(valueChanged(int)),          this,   SLOT(intSlot(int)));                    Q_ASSERT(ok);
    ok = connect(m_cannymax,    SIGNAL(valueChanged(int)),          this,   SLOT(intSlot(int)));                    Q_ASSERT(ok);
    ok = connect(m_eqcb,        SIGNAL(toggled(bool)),              this,   SLOT(boolSlot(bool)));                  Q_ASSERT(ok);
    ok = connect(&m_recalctimer,SIGNAL(timeout()),                  this,   SLOT(recalcSlot()));                    Q_ASSERT(ok);
    ok = connect(&m_cannytimer, SIGNAL(timeout()),                  this,   SLOT(recalcCannySlot()));               Q_ASSERT(ok);
    ok = connect(m_contourlist, SIGNAL(selected(const SScContour&)),this,   SLOT(contourSlot(const SScContour&)));  Q_ASSERT(ok);
}

HuNetMainWindow::~HuNetMainWindow()
{
    m_recalctimer.stop();
}

void HuNetMainWindow::doubleSlot(double)
{
    recalc();
}

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
        recalcCanny();
    }
    else if (sender()==m_cannymax)
    {
        if (m_cannymin->value()>=v)
        {
            m_cannymin->blockSignals(true);
            m_cannymin->setValue(v-1);
            m_cannymin->blockSignals(false);
        }
        recalcCanny();
    }
}

void HuNetMainWindow::boolSlot(bool)
{
    recalc();
}

void HuNetMainWindow::stringSlot(const QString&)
{
    recalc();
}

void HuNetMainWindow::recalcSlot()
{
    m_recalctimer.stop();
    m_cannytimer. stop();
    SScCannySetting sc(m_mediansb->value(), m_eqcb->isChecked());
    SScCannyContainer cc(m_dropper->filename(),sc);
    if (cc.isValid())
    {
        m_cc = cc;
        m_procdisplay->set(m_cc.orig());
        m_w1->show();
        m_procdisplay->show();
        recalcCannySlot();
    }
}

void HuNetMainWindow::recalcCannySlot()
{
    m_cannytimer.stop();
    if (m_cc.isValid())
    {
        auto c = m_cc.contours(m_cannymin->value(), m_cannymax->value());
        m_cannydisplay->set(m_cc.canny());
        m_w2->show();
        m_cannydisplay->show();
        m_contourlist->set(c);
    }
}

void HuNetMainWindow::contourSlot(SScContour c)
{
    m_contourdisplay->set(c.draw(256));
    if (c.size()==0) m_contourdisplay->hide(); else m_contourdisplay->show();
}
