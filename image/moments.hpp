#ifndef MOMENTS_HPP
#define MOMENTS_HPP

#include "matrix.hpp"
#include <QPair>
#include <QMap>
template <typename T> class SScMoments
{
public:
    SScMoments(const SScMatrix<T>& m);
    SScMoments(const QVector<QPair<int,int> >& pos, const QVector<T>& v = QVector<T>());
    /*!
     * \brief Raw moment
     * \param i
     * \param j1
     * \return
     */
    double getM(int i, int j);
    /*!
     * \brief Central moment
     * \param i
     * \param j
     * \return
     */
    double getMu(int i, int j);
    /*!
     * \brief Scale invariants
     * \param i
     * \param j
     * \return
     */
    double getEta(int i, int j);
    /*!
     * \brief Hu moment
     * \param i 1..7
     * \return
     */
    double getHu(int i);
    /*!
     * \brief normalized Hu -sign(hu)log(abs(hu))
     * \param i
     * \return
     */
    double getNHu(int i);

    void dump();

private:
    inline double xMean() { const double m00 = getM(0,0); return (m00!=0) ? getM(1,0)/m00 : 0.0; }
    inline double yMean() { const double m00 = getM(0,0); return (m00!=0) ? getM(0,1)/m00 : 0.0; }

    QMap<QPair<int,int>,double> moment_m, moment_mu, moment_eta;
    QMap<int,double> m_hu, m_nhu;
    SScMatrix<T> m_mtx;
    QVector<QPair<int,int> > m_pos;
    QVector<T> m_v;    
};

#endif // MOMENTS_HPP
