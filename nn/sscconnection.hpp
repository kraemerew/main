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
        ConnectionType_Simple,  //< Fixed learning rate
        Connectiontype_Momentum,   //< Momentum

        Connectiontype_RPROP,    //< exp increment, exp decrement = Resilient Backpropagation
        Connectiontype_LPLM,     //< linear increment, linear decrement
        Connectiontype_LPEM      //< linear increment, exp decrement
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
