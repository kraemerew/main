#ifndef SSCCONNECTION_HPP
#define SSCCONNECTION_HPP

class SScConnectionPrivate;

class SScConnection
{
public:
    enum SSeConnectionType
    {
        ConnectionType_Simple,  //< Fixed learning rate
        Connectiontype_RPROP,   //< Resilient Backpropagation
        Connectiontype_LPEM     //< linear increment, exp decrement
    };

    explicit SScConnection(SSeConnectionType type, double v, double initmin, double initmax, double eta);
    virtual ~SScConnection();

    void update(double dlt);
    SSeConnectionType type() const;
    void init();
    void trainingReset();
    double value() const;

private:
    SScConnectionPrivate* d_ptr;
};

#endif // SSCCONNECTION_HPP
