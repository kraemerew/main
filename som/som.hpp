#ifndef SSSOM_HPP
#define SSSOM_HPP

#include <QVector>
#include "../nn/sscactivation.hpp"

class SScSOMNeuron;

class SScSOMNetwork
{
public:
    /*!
     * \brief SScSOMNetwork
     * \param elements  Clusters
     * \param dim       Dimensionality of pattern space
     */
    SScSOMNetwork(int elements, int dim);
    virtual ~SScSOMNetwork();
    /*!
     * \brief Change the activation
     * \param type
     */
    void setActivation(SScActivation::SSeActivation type);
    /*!
     * \brief Training step
     * \param v Pattern
     * \param nr    Number of neurons to train negatively
     */
    void step(const QVector<double>& v, int nr);
    /*!
     * \brief Alternative training which will only train the winner and all the others negatively
     * \param v
     */
    void step(const QVector<double>& v);

private:
    QVector<SScSOMNeuron*> m_el;
};
#endif // SSSOM_HPP
