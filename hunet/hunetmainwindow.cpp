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
    : QTabWidget        (0),
      m_dropper         (new (std::nothrow) HuNetImageDropper()),
      m_procdisplay     (new (std::nothrow) HuNetImageDisplay()),
      m_cannydisplay    (new (std::nothrow) HuNetImageDisplay()),
      m_contourlist     (new (std::nothrow) HuNetContourList ()),
      m_contourdisplay  (new (std::nothrow) HuNetImageDisplay()),
      m_mediansb        (new (std::nothrow) QDoubleSpinBox   ()),
      m_eqcb            (new (std::nothrow) QCheckBox        ()),
      m_cannymin        (new (std::nothrow) QSpinBox         ()),
      m_cannymax        (new (std::nothrow) QSpinBox         ())
{    
    Q_CHECK_PTR(m_dropper);
    Q_CHECK_PTR(m_procdisplay);
    Q_CHECK_PTR(m_cannydisplay);
    Q_CHECK_PTR(m_contourlist);
    Q_CHECK_PTR(m_contourdisplay);
    Q_CHECK_PTR(m_mediansb);
    Q_CHECK_PTR(m_eqcb);
    Q_CHECK_PTR(m_cannymin);
    Q_CHECK_PTR(m_cannymax);

    m_procdisplay->insert(m_mediansb);
    m_procdisplay->insert(m_eqcb);
    m_procdisplay->insertStretch();

    m_cannydisplay->insert(m_cannymin);
    m_cannydisplay->insert(m_cannymax);
    m_cannydisplay->insertStretch();

    m_contourdisplay->insert(m_contourlist);


    m_dropper       ->insertInto(this,"Original");
    m_procdisplay   ->insertInto(this,"Processing");
    m_cannydisplay  ->insertInto(this,"Canny");
    m_contourdisplay->insertInto(this,"Contours");


    m_mediansb->setSingleStep(0.1);
    m_mediansb  ->setPrefix("Median: ");
    m_mediansb  ->setSuffix(("%"));
    m_eqcb      ->setText("Histogram Eq.");
    m_cannymin  ->setPrefix("Canny lower ");
    m_cannymax  ->setPrefix("Canny upper ");

    m_mediansb->setRange(0,99);
    m_cannymin->setRange(0,254);
    m_cannymax->setRange(1,255);

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
    m_orig = m_dropper->get();
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
        m_orig = m_cannydisplay->get();
        m_orig=m_orig.convertToFormat(QImage::Format_RGB32);
        m_contourlist->set(c);
    }
}

void HuNetMainWindow::contourSlot(SScContour c)
{
    m_contourdisplay->set(c.draw(256));
    if (c.size()==0) m_contourdisplay->hide(); else m_contourdisplay->show();
    QImage im = m_orig;
    if (c.draw(im,3))
    {
       m_cannydisplay->set(im);
    }
}
