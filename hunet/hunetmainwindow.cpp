#include "hunetmainwindow.hpp"
#include "imageloader.hpp"
#include "hunetimagedisplay.hpp"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

HuNetMainWindow::HuNetMainWindow()
    : QWidget           (0),
      m_tab             (new (std::nothrow) QTabWidget      (this)),
      m_loader          (new (std::nothrow) HuNetImageLoader    ()),
      m_procdisplay     (new (std::nothrow) HuNetImageDisplay   ()),
      m_cannydisplay    (new (std::nothrow) HuNetImageDisplay   ()),
      m_contourdisplay  (new (std::nothrow) SScContourContainer ()),
      m_mediansb        (new (std::nothrow) QDoubleSpinBox      ()),
      m_bilcsigma       (new (std::nothrow) QDoubleSpinBox      ()),
      m_bilsigma        (new (std::nothrow) QDoubleSpinBox      ()),
      m_eqcb            (new (std::nothrow) QCheckBox           ()),
      m_bilcb           (new (std::nothrow) QCheckBox           ()),
      m_cannymin        (new (std::nothrow) QSpinBox            ()),
      m_cannymax        (new (std::nothrow) QSpinBox            ()),
      m_bild            (new (std::nothrow) QSpinBox            ())
{    
    Q_CHECK_PTR(m_tab);
    Q_CHECK_PTR(m_loader);
    Q_CHECK_PTR(m_procdisplay);
    Q_CHECK_PTR(m_cannydisplay);
    Q_CHECK_PTR(m_contourdisplay);
    Q_CHECK_PTR(m_mediansb);
    Q_CHECK_PTR(m_eqcb);
    Q_CHECK_PTR(m_cannymin);
    Q_CHECK_PTR(m_cannymax);
    Q_CHECK_PTR(m_bilcb);
    Q_CHECK_PTR(m_bilsigma);
    Q_CHECK_PTR(m_bilcsigma);
    Q_CHECK_PTR(m_bild);

    setLayout(new QHBoxLayout);
    Q_CHECK_PTR(layout());
    layout()->addWidget(m_tab);
    layout()->addWidget(m_contourdisplay->list());

    m_contourdisplay->insertStretch();
    m_procdisplay->insert(m_bilcb);
    m_procdisplay->insert(m_bild);
    m_procdisplay->insert(m_bilcsigma);
    m_procdisplay->insert(m_bilsigma);
    m_procdisplay->insert(m_mediansb);
    m_procdisplay->insert(m_eqcb);
    m_procdisplay->insertStretch();

    m_cannydisplay->insert(m_cannymin);
    m_cannydisplay->insert(m_cannymax);
    m_cannydisplay->insertStretch();

    m_loader       ->insertInto(m_tab,"Original");
    m_procdisplay   ->insertInto(m_tab,"Processing");
    m_cannydisplay  ->insertInto(m_tab,"Canny");
    m_contourdisplay->insertInto(m_tab,"Contours");

    m_bild->setRange(1,5);
    m_bilcsigma->setRange(1,1000);
    m_bilsigma->setRange(1,1000);

    m_mediansb  ->setSingleStep(0.1);
    m_mediansb  ->setPrefix ("Median: ");
    m_mediansb  ->setSuffix (("%"));
    m_eqcb      ->setText   ("Histogram Eq.");
    m_cannymin  ->setPrefix ("Canny lower ");
    m_cannymax  ->setPrefix ("Canny upper ");
    m_bilcb->setText("Bilateral Filter");
    m_bild->setPrefix("Bil. Distance");
    m_bilcsigma->setPrefix("Bil. Color Sigma");
    m_bilsigma->setPrefix("Bil. Sigma");
    m_mediansb->setRange(0,99);
    m_cannymin->setRange(0,254);
    m_cannymax->setRange(1,255);
    m_bilcb->setChecked(true);
    m_cannymin  ->setValue  (50);
    m_cannymax  ->setValue  (250);
    m_eqcb      ->setChecked(false);
    m_mediansb  ->setValue(1.0);

    bool ok;
    Q_UNUSED(ok);
    ok = connect(m_loader,                  SIGNAL(loaded(const QString&)),     this,       SLOT(stringSlot(const QString&)));  Q_ASSERT(ok);
    ok = connect(m_mediansb,                SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_cannymin,                SIGNAL(valueChanged(int)),          this,       SLOT(intSlot(int)));                Q_ASSERT(ok);
    ok = connect(m_cannymax,                SIGNAL(valueChanged(int)),          this,       SLOT(intSlot(int)));                Q_ASSERT(ok);
    ok = connect(m_bild,                    SIGNAL(valueChanged(int)),          this,       SLOT(intSlot(int)));                Q_ASSERT(ok);
    ok = connect(m_bilcb,                   SIGNAL(toggled(bool)),              this,       SLOT(boolSlot(bool)));              Q_ASSERT(ok);
    ok = connect(m_bilsigma,                SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_bilcsigma,               SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_eqcb,                    SIGNAL(toggled(bool)),              this,       SLOT(boolSlot(bool)));              Q_ASSERT(ok);
    ok = connect(&m_recalctimer,            SIGNAL(timeout()),                  this,       SLOT(recalcSlot()));                Q_ASSERT(ok);
    ok = connect(&m_cannytimer,             SIGNAL(timeout()),                  this,       SLOT(recalcCannySlot()));           Q_ASSERT(ok);
    ok = connect(m_contourdisplay->list(),  SIGNAL(selected(const SScContour&)),this,       SLOT(contourSlot(SScContour)));     Q_ASSERT(ok);
    ok = connect(m_loader,                  SIGNAL(dropped(const QString&)),    m_loader,   SLOT(tryLoad(const QString&)));     Q_ASSERT(ok);
    ok = connect(m_cannydisplay,            SIGNAL(dropped(const QString&)),    m_loader,   SLOT(tryLoad(const QString&)));     Q_ASSERT(ok);
    ok = connect(m_contourdisplay,          SIGNAL(dropped(const QString&)),    m_loader,   SLOT(tryLoad(const QString&)));     Q_ASSERT(ok);
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
    else recalc();
}

void HuNetMainWindow::boolSlot(bool)
{
    m_bilsigma->setEnabled(m_bilcb->isChecked());
    m_bilcsigma->setEnabled(m_bilcb->isChecked());
    m_bild->setEnabled(m_bilcb->isChecked());

    recalc();
}

void HuNetMainWindow::stringSlot(const QString&)
{
    m_orig = m_loader->get();
    recalc();
}

void HuNetMainWindow::recalcSlot()
{
    m_recalctimer.stop();
    m_cannytimer. stop();
    SScCannySetting sc(m_mediansb->value(), m_eqcb->isChecked(), m_bilcb->isChecked(), m_bild->value(), m_bilcsigma->value(),m_bilsigma->value());
    SScCannyContainer cc(m_loader->filename(),sc);
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
        m_contourdisplay->setContours(c);
    }
}

void HuNetMainWindow::contourSlot(SScContour c)
{
    QImage im = m_orig;
    if (c.mark(im,3))
    {
       m_cannydisplay->set(im);
    }
}
