#ifndef SSNETWORK_HPP
#define SSNETWORK_HPP

#include "sscnetwork_global.h"
#include "sscneuron.hpp"

#include <QMap>
class SSNETWORKSHARED_EXPORT SScNetwork
{
public:
    SScNetwork();
private:
    QMap<int,SScNeuron*>  m_neurons;
};

#endif // SSNETWORK_HPP
