#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "hunetimagedisplay.hpp"

class HuNetImageLoader : public HuNetImageDisplay
{
    Q_OBJECT

public:
    explicit HuNetImageLoader(QWidget* parent = NULL);
    virtual ~HuNetImageLoader();
    inline QString filename() const { return m_filename; }

    virtual bool allowDrops() const { return true; }

public slots:
    bool tryLoad(const QString& filename);

signals:
    void loaded(const QString&);

private:
    QString m_filename;
    QLabel* m_label;
};

#endif // IMAGELOADER_HPP
