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
namespace SSnConvHelpers
{
    /*!
     * \brief Determin the input array size for an input conv unit given the number of x/y units, kernel size and overlap
     * \param kx
     * \param ky
     * \param ovl
     * \param xunits
     * \param yunits
     * \return
     */
    QSize inputSize(int kx, int ky, int ovl, int xunits, int yunits);
    /*!
     * \brief Convenience call, returning positions converted to linear array index (don't use on image classes like QImage because they may have padding)
     * \param kx
     * \param ky
     * \param ovl
     * \param xidx
     * \param yidx
     * \param w
     * \param h
     * \return
     */
    QList<int> convMaskIndexes(int kx, int ky, int ovl, int xidx, int yidx, int w, int h);
    /*!
     * \brief Return convolution input as list of points, given kernel parameters and input image wxh as well as mask index
     * \param kx    Kernel size x
     * \param ky    Kernel size y
     * \param ovl   Overlap
     * \param xidx  Mask index x, translates to a position in input array based on kx,ky,ovl - this is actually the output neuron position
     * \param yidx  Mask index y - same
     * \param w     Source image width
     * \param h     Source image height
     * \return
     */
    QList<QPoint> convMaskPositions(int kx, int ky, int ovl, int xidx, int yidx, int w, int h);
    /*!
     * \brief Checks whether a convolution mask of kx,ky with overlap ovl fits into an image/array of wxh
     * \param kx    Applied kernel width
     * \param ky    Applied kernel height
     * \param ovl   Overlap in kernel
     * \param w     Source array width
     * \param h     Source array height
     * \return  Number of kernel units or invalid size on failure
     */
    QSize convMaskFits(int kx, int ky, int ovl, int w, int h);
}

#endif // CONVHELPERS_HPP
