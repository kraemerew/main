#ifndef CONVIMAGECONVERTER_HPP
#define CONVIMAGECONVERTER_HPP

#include <QString>
#include <QImage>
#include <QSize>
#include <QPoint>
#include <QVector>

class SScConvImageConverter
{
public:
    /*!
     * \brief SScConvImageConverter
     * \param kx        Kernel width            (>0)
     * \param ky        Kernel height           (>0)
     * \param overlap   Overlap                 (>=0)
     * \param outw      Output layer columns    (>0)
     * \param outh      Output layer lines      (>0)
     * \param color     Use color image
     * \param file      File to load
     */
    SScConvImageConverter(int kx, int ky, int overlap, int outw, int outh, bool color, const QString& file);
    /*!
     * \brief Convenience constructor
     * \param kx
     * \param ky
     * \param overlap
     * \param outw
     * \param outh
     * \param color
     * \param im
     */
    SScConvImageConverter(int kx, int ky, int overlap, int outw, int outh, bool color, const QImage& im);

    /*!
     * \brief Check validity - file loaded, parameters valid
     * \return
     */
    inline bool     isValid     () const                { return m_valid; }
    /*!
     * \brief Return input pixel value vector for output layer element
     * \param x Element x (>=0)
     * \param y Element y (>=0)
     * \return kx*ky*depth elements
     */
    QVector<double> pattern     (int x, int y) const;
    /*!
     * \brief Cut image input for output element x,y
     * \param x
     * \param y
     * \return
     */
    QImage cut(int x, int y) const;

private:
    inline int      inputWidth  () const                { return (m_outw*m_kx) - ((m_outw-1)*m_ovl); }
    inline int      inputHeight () const                { return (m_outh*m_ky) - ((m_outh-1)*m_ovl); }
    inline QSize    inputSize   () const                { return QSize(inputWidth(), inputHeight()); }
    inline QSize    kernelSize  () const                { return QSize(m_kx, m_ky); }
    inline int      topLeftX    (int x) const           { return x*m_kx - m_ovl*x; }
    inline int      topLeftY    (int y) const           { return y*m_ky - m_ovl*y; }
    inline QPoint   topLeft     (int x, int y) const    { return QPoint(topLeftX(x), topLeftY(y)); }
    inline int      depth       () const                { return m_color ? 3 : 1; }
    inline int      dim         () const                { return m_kx*m_ky*depth(); }
    void            init        ();

    QImage m_im;
    int m_kx, m_ky, m_ovl, m_outw, m_outh;
    bool m_color, m_valid;
};

#endif // CONVIMAGECONVERTER_HPP
