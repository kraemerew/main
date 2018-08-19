#ifndef FRAMEINTERVALDESCRIPTOR_HPP
#define FRAMEINTERVALDESCRIPTOR_HPP

#include <QtGlobal>
#include <QPair>
class SScFrameIntervalDescriptor
{
public:
    /*!
     * \brief Discrete version
     * \param n
     * \param d
     */
    SScFrameIntervalDescriptor(quint32 n, quint32 d);
    /*!
     * \brief Stepwise or continuous descriptor
     * \param n_min
     * \param d_min
     * \param n_max
     * \param d_max
     * \param n_step
     * \param d_step
     */
    SScFrameIntervalDescriptor(quint32 n_min,quint32 d_min, quint32 n_max, quint32 d_max, quint32 n_step, quint32 d_step);
    /*!
     * \brief allowed
     * \param n
     * \param d
     * \return
     */
    virtual bool allowed(quint32 n, quint32 d) const;
    /*!
     * \brief Fit next best
     * \param n
     * \param d
     * \return
     */
    virtual QPair<quint32,quint32> fit(quint32 n, quint32 d) const;
    /*!
     * \brief Mode - single value or step
     * \return
     */
    inline bool discrete() const { return (m_nmin==m_nmax) && (m_dmin==m_dmax); }
    /*!
     * \brief Dump to shell
     */
    void dump() const;
    /*!
     * \brief Find the best fit for the frame interval over a set of sescriptors
     * \param n
     * \param d
     * \return
     */
    static QPair<quint32,quint32> fit(quint32 n, quint32 d, const QList<SScFrameIntervalDescriptor>& l);

private:
    quint32 m_nmin, m_dmin, m_nmax, m_dmax, m_nstep, m_dstep;
};

#endif // FRAMEINTERVALDESCRIPTOR_HPP
