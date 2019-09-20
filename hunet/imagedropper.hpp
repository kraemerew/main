#ifndef IMAGEDROPPER_HPP
#define IMAGEDROPPER_HPP

#include "hunetimagedisplay.hpp"

class HuNetImageDropper : public HuNetImageDisplay
{
    Q_OBJECT

public:
    explicit HuNetImageDropper(QWidget* parent = NULL);
    virtual ~HuNetImageDropper();
    inline QString filename() const { return m_filename; }

    virtual bool allowDrops() const { return true; }
    virtual bool dropped(const QString &filename);

signals:
    void loaded(const QString&);

private:
    QString m_filename;
    QLabel* m_label;
};

#endif // IMAGEDROPPER_HPP
