#ifndef CONTOUR_HPP
#define CONTOUR_HPP

#include <QVariantMap>
#include <QImage>
#include <vector>
#include <opencv2/core.hpp>

class SScContour
{
public:
    SScContour();
    SScContour(const std::vector<cv::Point>& v);
    SScContour(const QVariantMap& vm);

    QVariantMap vm() const;
    inline bool isTagged() const { return !tag().isEmpty(); }
    QString tag() const;
    void setTag(const QString& newtag);
    QString md5();

    inline void     reserve (size_t sz) { m_data.reserve(sz); }
    inline size_t   size    () const { return m_data.size(); }
    inline bool     isEmpty () const { return size()==0; }
    inline bool     isValid () const { return size()>1; }
    inline void     append(const cv::Point& p) { m_data.insert(m_data.end(), p); }
    QImage draw(int w = 255, double th = 2.0, const QColor& c = Qt::white, bool closed = false) const;
    bool draw(QImage& im, double th = 2.0, const QColor& c = Qt::white, bool closed = false) const;
    bool mark(QImage&, double th = 1.5) const;
    double* huMoments();
    double perimeter() const;
    /*!
     * \brief Area div by perimeter
     * \return
     */
    inline double hydraulicRadius() const { const double a = area(), p = perimeter(); if (p==0) return 0; return a/p; }
    /*!
     * \brief Convec perimeter divided by
     * \return
     */
    inline double convexity() const { return isEmpty() ? -1 : convexHull().perimeter()/perimeter(); }
    /*!
     * \brief Heywood circularity factor (perimeter div by perimeter of circle of same area)
     * \return
     */
    double heywoodCFact() const;
    /*!
     * \brief Area div by bounding rect area
     * \return
     */
    double compactnessFactor() const;
    /*!
     * \brief Bounding rect elongation
     * \return
     */
    double elongationFactor() const;
    /*!
     * \brief Radius of disk with same area
     * \return
     */
    double waddelDiskRadius() const;
    QPair<int,int> xRange() const;
    QPair<int,int> yRange() const;
    bool isSelfIntersected(bool closed=true) const;
    double circularity() const;

    static QStringList contourLabels();
    SScContour contour(const QString& id) const;
    static QStringList featureLabels();
    QStringList featureValues();

    SScContour fitEllipse() const;
    SScContour convexHull() const;
    SScContour concaveHull() const;
    SScContour hull(double epsilon = 1.0) const;
    SScContour approxHull(int nr, int steps = 10000) const;
    SScContour approxConvexHull(int nr, int steps = 10000) const;

    static QVariantMap toVM(const QList<SScContour>& cl);
    static QString toJson(const QList<SScContour>& cl);
    static QList<SScContour> fromJson(const QString&);

private:
    QList<QLineF> lines(bool closed) const;
    SScContour norm(double w) const;
    double minEnclosingCircleRadius () const;
    double convexHullArea           () const;
    double area                     () const;
    double hullArea                 (double epsilon = 1.0) const;

    std::vector<cv::Point>  m_data;
    bool                    m_done;
    double                  m_hu[7];
    QString                 m_md5, m_tag;
};

class SScContourSet : public QMap<QString,SScContour>
{
public:
    explicit SScContourSet(const QList<SScContour>& cl = QList<SScContour>());
    explicit SScContourSet(const QString& jsondata);
    bool add(const SScContour& c);
    void add(const QList<SScContour>& cl);
    void add(const SScContourSet& other);
    inline QList<SScContour> toList() const { return values(); }
    void fromJson(const QString& data, bool merge = false);
    QString toJSon() const;

    bool tag(const QString& md5, const QString& tag);
    QString tag(const QString& md5) const;
};

#endif // CONTOUR_HPP
