#ifndef HUNETMAINWINDOW_HPP
#define HUNETMAINWINDOW_HPP

#include <QWidget>

class HuNetImageDropper;
class HuNetCannyDisplay;
class HuNetContourList;

class HuNetMainWindow : public QWidget
{
public:
    explicit HuNetMainWindow();
    virtual ~HuNetMainWindow();

private:
    HuNetImageDropper*  m_dropper;
    HuNetCannyDisplay*  m_cannydisplay;
    HuNetContourList*   m_contourlist;
};

#endif // HUNETMAINWINDOW_HPP
