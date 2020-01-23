#ifndef CONVHELPERS_HPP
#define CONVHELPERS_HPP

#include <QSize>
#include <QList>
#include <QPoint>
#include <QImage>
#include <QRgb>

namespace SSnConvHelper
{
/*!
 * \brief Create normalized image
 * \param v
 * \param sz
 * \return
 */
QImage toImage(const QVector<double>& v, const QSize& sz);
/*!
 * \brief For a given element in an output array, returns the connection indexes
 * \param stride    Kernel stride x/y
 * \param outputs   Output array size
 * \param element   Element reference
 * \return
 */
QList<QPoint> convPositions(const QSize& kernel, const QSize& stride, const QPoint& element);
/*!
 * \brief Return filter convolution start top left
 * \param kernel
 * \param stride
 * \param outputs
 * \param element
 * \return
 */
QPoint topLeftPosition(const QSize& stride, const QPoint& element);
/*!
 * \brief For a given output size, kernel size and stride, return input resolution
 * \param kernel
 * \param stride
 * \param outputsize
 * \return
 */
QSize inputSize(const QSize& kernel, const QSize& stride, const QSize& outputsize);
/*!
 * \brief Check sanity of values
 * \param kernel    Kernel size
 * \param stride    Kernel stride
 * \return
 */
bool isSane(const QSize& kernel, const QSize& stride);
/*!
 * \brief Convenience call adding the output size
 * \param kernel
 * \param stride
 * \param outputsize
 * \return
 */
bool isSane(const QSize& kernel, const QSize& stride, const QSize& outputsize);
/*!
 * \brief Return image scaled to input size of a kernel,stride,outputsize setting
 * \param im
 * \param kernel
 * \param stride
 * \param outputsize
 * \return
 */
QImage scaledImage(const QImage& im, const QSize& kernel, const QSize& stride, const QSize& outputsize);
/*!
 * \brief Convert stride to overlap
 * \param kernel
 * \param stride
 * \return
 */
QSize stride2Overlap(const QSize& kernel, const QSize& stride);
/*!
 * \brief Convert overlap to stride - same call as stride2Overlap
 * \param kernel
 * \param overlap
 * \return
 */
QSize overlap2Stride(const QSize& kernel, const QSize& overlap);
/*!
 * \brief Return
 * \param im
 * \param kernel
 * \param stride
 * \param outputsize
 * \return list of inputs for the elements (line-wise)
 */
QList<QList<QRgb> > matrix(const QImage& im, const QSize& kernel, const QSize& stride, const QSize& outputsize);
/*!
 * \brief Convert to color value matrix (lines will be 3 times the size of input for r,g,b)
 * \param matrix
 * \return
 */
QList<QList<uchar> > colorValues(const QList<QList<QRgb> >& matrix);
/*!
 * \brief Convert to grayvalue matrix
 * \param matrix
 * \return
 */
QList<QList<uchar> > grayValues(const QList<QList<QRgb> >& matrix);
/*!
 * \brief Returns a cutout of an (rescaled) input image for a given neuron
 * \param im
 * \param kernel
 * \param stride
 * \param outputsize
 * \param element       The output element
 * \return Input cutout for the given element
 */
QImage image(const QImage& im, const QSize& kernel, const QSize& stride, const QSize& outputsize, const QPoint& element);
/*!
 * \brief Return a list of cutout images acting as input to the output array
 * \param im
 * \param kernel
 * \param stride
 * \param outputsize
 * \param gray
 * \return
 */
QList<QImage> images(const QImage& im, const QSize& kernel, const QSize& stride, const QSize& outputsize);
/*!
 * \brief Convert image to RGB list
 * \param im
 * \return
 */
QList<QRgb> im2List(QImage& im);
/*!
 * \brief fitToInput
 * \param input
 * \param kernel
 * \param stride
 * \return Number of output elements or -1,-1 on failure
 */
QSize fitToInput(const QSize& input, const QSize& kernel, const QSize& stride);
}



class SScConvSetting
{
public:
    /*!
     * \brief SScConvSetting
     * \param kernel    Kernel size
     * \param stride    Kernel stride
     * \param output    Output elements
     */
    explicit SScConvSetting(const QSize& kernel, const QSize& stride, const QSize& output);
    /*!
     * \brief Create based on a kernel stride setting and another input convolution layer
     * \param kernel
     * \param stride
     * \param input
     */
    explicit SScConvSetting(const QSize& kernel, const QSize& stride, const SScConvSetting& input);

    inline bool     isValid     () const { return kernelValid() && strideValid() && inputValid() && outputValid(); }
    inline bool     kernelValid () const { return (m_k.width()>1) && (m_k.height()>1); }
    inline bool     strideValid () const { return (m_s.width()>0) && (m_s.height()>0); }
    inline bool     inputValid  () const { return (m_i.width()>0) && (m_i.height()>0); }
    inline bool     outputValid () const { return (m_o.width()>0) && (m_o.height()>0); }

    inline QSize    overlap     () const { return SSnConvHelper::stride2Overlap(kernel(),stride()); }
    inline QSize    kernel      () const { return m_k; }
    inline QSize    stride      () const { return m_s; }
    inline QSize    input       () const { return m_i; }
    inline QSize    output      () const { return m_o; }

    inline bool     canInputFrom(const SScConvSetting& other) const { return input()==other.output(); }
    inline bool     canOutputTo (const SScConvSetting& other) const { return output()==other.input(); }

private:
    QSize m_k, m_s, m_o, m_i;
};

#endif // CONVHELPERS_HPP
