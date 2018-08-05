#ifndef SSCCONNECTION_HPP
#define SSCCONNECTION_HPP

class SScConnectionPrivate;
#include <Qt>
class SScConnection
{
    Q_DISABLE_COPY(SScConnection)

public:
    enum SSeConnectionType
    {
        ConnectionType_Simple,   //< Fixed learning rate
        Connectiontype_Momentum, //< Momentum
        Connectiontype_RPROP,    //< Resilient Backpropagation
        Connectiontype_LPLM,     //< linear increment, linear decrement
        Connectiontype_LPEM,     //< linear increment, exp decrement
        Connectiontype_EPEM,     //< exp increment, exp decrement

        Connectiontype_RMSProp,  //< RMSProp algorithm
        Connectiontype_Adam,     //< ADAM algorithm
        Connectiontype_AMSGrad   //< ADAM with second order using the maximum
    };

    explicit SScConnection(SSeConnectionType type, double v, double initmin = -1, double initmax = 1, double eta = 0.1);
    virtual ~SScConnection();
    void trainingReset();
    void update (double dlt, bool cycleDone);
    SSeConnectionType type() const;
    void init();

    double value() const;

private:
    SScConnectionPrivate* d_ptr;
};

#endif // SSCCONNECTION_HPP
