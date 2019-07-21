#include "conv.hpp"

SScConvNeuron::SScConvNeuron(SScHighwayNetwork *net)
    :   SSiHighwayNeuron(net, Conv, SScActivation::SWISH),
        m_cu            (NULL),
        m_cuidx         (-1)
{}

SScConvNeuron::~SScConvNeuron()
{}
