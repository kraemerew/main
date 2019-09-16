#include "hunetmainwindow.hpp"
#include "imagedropper.hpp"
#include "hunetcannydisplay.hpp"
#include "hunetcontourlist.hpp"
#include <QHBoxLayout>
HuNetMainWindow::HuNetMainWindow()
    : QWidget       (0),
      m_dropper     (new (std::nothrow) HuNetImageDropper(this)),
      m_cannydisplay(new (std::nothrow) HuNetCannyDisplay(this)),
      m_contourlist (new (std::nothrow) HuNetContourList (this))
{
    QHBoxLayout* l = new (std::nothrow) QHBoxLayout;
    setLayout(l);
    l->addWidget(m_dropper);
    l->addWidget(m_cannydisplay);
    l->addWidget(m_contourlist);
    m_cannydisplay  ->hide();
    m_contourlist   ->hide();
}

HuNetMainWindow::~HuNetMainWindow()
{
}
