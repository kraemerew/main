#include "hunetmainwindow.hpp"
#include "imageloader.hpp"
#include "hunetimagedisplay.hpp"
#include "watershed.hpp"

#include <QFileDialog>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>

HuNetMainWindow::HuNetMainWindow()
    : QWidget           (0),
      m_tab             (new (std::nothrow) QTabWidget      (this)),
     // m_loader          (new (std::nothrow) HuNetImageLoader    ()),
      m_procdisplay     (new (std::nothrow) HuNetImageDisplay   ()),
      m_contourdisplay  (new (std::nothrow) SScContourContainer ()),
      m_imgsel          (new (std::nothrow) QComboBox           ()),
      m_mediansb        (new (std::nothrow) QDoubleSpinBox      ()),
      m_distsb          (new (std::nothrow) QDoubleSpinBox      ()),
      m_closesb         (new (std::nothrow) QDoubleSpinBox      ()),
      m_invcb           (new (std::nothrow) QCheckBox           ()),
      m_binthr          (new (std::nothrow) QSpinBox            ()),
      m_tagpos          (new (std::nothrow) QPushButton         ()),
      m_tagneg          (new (std::nothrow) QPushButton         ())
{    
    Q_CHECK_PTR(m_tab);
    //Q_CHECK_PTR(m_loader);
    Q_CHECK_PTR(m_procdisplay);
    Q_CHECK_PTR(m_contourdisplay);
    Q_CHECK_PTR(m_imgsel);
    Q_CHECK_PTR(m_mediansb);
    Q_CHECK_PTR(m_distsb);
    Q_CHECK_PTR(m_closesb);
    Q_CHECK_PTR(m_invcb);
    Q_CHECK_PTR(m_binthr);
    Q_CHECK_PTR(m_tagpos);
    Q_CHECK_PTR(m_tagneg);

    setLayout(new QHBoxLayout);
    Q_CHECK_PTR(layout());
    layout()->addWidget(m_tab);

    QWidget* w = new (std::nothrow) QWidget(this);
    w->setLayout(new (std::nothrow) QVBoxLayout(w));
    layout()->addWidget(w);

    QWidget* ctlgroup = new (std::nothrow) QWidget(w);
    ctlgroup->setLayout(new (std::nothrow) QHBoxLayout);
    QWidget* taggroup = new (std::nothrow) QWidget(w);
    taggroup->setLayout(new (std::nothrow) QHBoxLayout);

    QPushButton* lb = new QPushButton(ctlgroup);
    QPushButton* sb = new QPushButton(ctlgroup);
    lb->setText("Load");
    sb->setText("Save");
    ctlgroup->layout()->addWidget(lb);
    ctlgroup->layout()->addWidget(sb);

    m_tagpos->setText("Positive");
    m_tagneg->setText("Negaive");
    m_tagpos->setCheckable(true);
    m_tagneg->setCheckable(true);
    taggroup->layout()->addWidget(new QLabel("Tagging"));
    taggroup->layout()->addWidget(m_tagpos);
    taggroup->layout()->addWidget(m_tagneg);

    w->layout()->addWidget(ctlgroup);
    w->layout()->addWidget(m_contourdisplay->list());
    w->layout()->addWidget(taggroup);

    m_contourdisplay->insertStretch();
    m_procdisplay->insert(m_imgsel);

    m_procdisplay->insert(m_mediansb);
    m_procdisplay->insert(m_invcb);
    m_procdisplay->insert(m_closesb);
    m_procdisplay->insert(m_binthr);
    m_procdisplay->insert(m_distsb);


    m_procdisplay->insertStretch();

    m_procdisplay   ->insertInto(m_tab,"Souurce");
    m_contourdisplay->insertInto(m_tab,"Contours");

    m_mediansb  ->setRange(0,99);
    m_mediansb  ->setSingleStep(0.1);
    m_mediansb  ->setPrefix ("Median: ");
    m_mediansb  ->setSuffix (("%"));
    m_mediansb  ->setValue(0);

    m_closesb  ->setRange(0,99);
    m_closesb  ->setSingleStep(0.1);
    m_closesb  ->setPrefix ("Closing: ");
    m_closesb  ->setSuffix (("%"));
    m_closesb  ->setValue(2);

    m_distsb  ->setRange(0,1);
    m_distsb  ->setSingleStep(0.1);
    m_distsb  ->setPrefix ("Distance thr: ");
    m_distsb  ->setValue(.4);
    m_invcb   ->setText("Invert");

    m_binthr  ->setRange(1,254);
    m_binthr  ->setSingleStep(1);
    m_binthr  ->setPrefix ("Bin Thr: ");
    m_binthr  ->setValue(128);

    m_imgsel->addItems(SScWatershedContainer::images());

    bool ok;
    Q_UNUSED(ok);
    ok = connect(m_mediansb,                SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_closesb,                 SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_distsb,                  SIGNAL(valueChanged(double)),       this,       SLOT(doubleSlot(double)));          Q_ASSERT(ok);
    ok = connect(m_binthr,                  SIGNAL(valueChanged(int)),          this,       SLOT(intSlot(int)));          Q_ASSERT(ok);
    ok = connect(m_imgsel, SIGNAL(currentIndexChanged(int)), this, SLOT(intSlot(int))); Q_ASSERT(ok);
    ok = connect(m_invcb,                   SIGNAL(toggled(bool)),              this,       SLOT(boolSlot(bool)));              Q_ASSERT(ok);

    ok = connect(&m_recalctimer,            SIGNAL(timeout()),                  this,       SLOT(recalcSlot()));                Q_ASSERT(ok);
    ok = connect(m_contourdisplay->list(),  SIGNAL(selected(const SScContour&)),this,       SLOT(contourSlot(SScContour)));     Q_ASSERT(ok);
    ok = connect(m_procdisplay,             SIGNAL(dropped(const QString&)),    this,       SLOT(tryLoadSlot(const QString&)));     Q_ASSERT(ok);
    ok = connect(m_contourdisplay,          SIGNAL(dropped(const QString&)),    this,       SLOT(tryLoadSlot(const QString&)));     Q_ASSERT(ok);
    ok = connect(lb,                        SIGNAL(clicked()),                  this,       SLOT(loadSlot()));                  Q_ASSERT(ok);
    ok = connect(sb,                        SIGNAL(clicked()),                  this,       SLOT(saveSlot()));                  Q_ASSERT(ok);
    ok = connect(m_tagpos,                  SIGNAL(toggled(bool)),              this,       SLOT(tagSlot(bool)));               Q_ASSERT(ok);
    ok = connect(m_tagneg,                  SIGNAL(toggled(bool)),              this,       SLOT(tagSlot(bool)));               Q_ASSERT(ok);
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
    if (sender()==m_imgsel)
    {
        m_procdisplay->set(m_wc.image(v));
    }
    else recalc();
}

void HuNetMainWindow::boolSlot(bool)
{
    recalc();
}

void HuNetMainWindow::tryLoadSlot(const QString& filename)
{
    if (filename!=m_filename)
    {
        m_filename = filename;
        recalcSlot();
    }
}

void HuNetMainWindow::recalcSlot()
{
    m_recalctimer.stop();
    const SSnWatershed::Pars p(m_mediansb->value(),m_closesb->value(),m_binthr->value(),m_invcb->isChecked(),m_distsb->value());

    SScWatershedContainer wc(m_filename,p);
    if (wc.isValid())
    {
        qWarning(">>>>SETTING");
        m_wc = wc;
        m_procdisplay->set(m_wc.image(m_imgsel->currentIndex()));
        m_contourdisplay->setContours(m_wc.contours());
    }
    else qWarning(">>>>>NOT VALID");
}

void HuNetMainWindow::contourSlot(SScContour c)
{
    if (c.tag()=="+") m_tagpos->setChecked(true); else
    if (c.tag()=="-") m_tagneg->setChecked(true); else
    {
        m_tagpos->setChecked(false);
        m_tagneg->setChecked(false);
    }
    //QImage im = m_procdisplay->image(m_imgsel->currentIndex());
    /*if (c.mark(im,3))
    {
       m_cannydisplay->set(im);
    }*/
}

void HuNetMainWindow::loadSlot()
{
    const QString fn = QFileDialog::getOpenFileName(NULL,"*.cnt");
    QFile f(fn);
    if (f.open(QIODevice::ReadOnly))
    {
        SScContourSet cc(f.readAll());
        m_contourdisplay->setContours(cc.toList());
    }
}

void HuNetMainWindow::saveSlot()
{
    const QString fn = QFileDialog::getSaveFileName(NULL,"*.cnt");
    QFile f(fn);
    SScContourSet cc;
    if (f.open(QIODevice::ReadOnly))
    {
        cc = SScContourSet(f.readAll());

        f.close();
    }
    if (f.open(QIODevice::WriteOnly))
    {
        cc.add(m_contourdisplay->getContours());
        f.write(cc.toJSon().toUtf8());
        f.close();
    }
}

void HuNetMainWindow::tagSlot(bool b)
{
    if (sender())
    {
        if (b)
        {
            if (sender()==m_tagpos) { m_contourdisplay->setTag("+"); m_tagneg->setChecked(false); } else
            if (sender()==m_tagneg) { m_contourdisplay->setTag("-"); m_tagpos->setChecked(false); }
        }
        else
        {
            if (!m_tagpos->isChecked() && !m_tagneg->isChecked()) m_contourdisplay->setTag(QString());
        }
    }
}
