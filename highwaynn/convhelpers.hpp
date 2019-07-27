#ifndef CONVHELPERS_HPP
#define CONVHELPERS_HPP

#include <QSize>
#include <QList>
#include <QPoint>

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
