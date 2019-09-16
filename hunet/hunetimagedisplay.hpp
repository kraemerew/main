#ifndef HUNETIMAGEDISPLAY_HPP
#define HUNETIMAGEDISPLAY_HPP

#include <QLabel>
#include <QImage>

class HuNetImageDisplay : public QLabel
{
public:
    HuNetImageDisplay(QWidget* parent = NULL);
    void set(const QImage&);
};

#endif // HUNETIMAGEDISPLAY_HPP
