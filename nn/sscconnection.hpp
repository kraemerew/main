#ifndef SSCCONNECTION_HPP
#define SSCCONNECTION_HPP


class SScConnection
{
public:
    enum SSeConnectionType
    {
        ConnectionType_Simple,  //< Fixed learning rate
        Connectiontype_RPROP,   //< Resilient Backpropagation
        Connectiontype_LPEM     //< linear increment, exp decrement
    };

    explicit SScConnection(SSeConnectionType type, double value, double initmin, double initmax, double eta);
    virtual ~SScConnection() {}
    static SScConnection create(SSeConnectionType type, bool& ok, double v, double initmin = -1, double initmax = 1, double eta = 0.1);

    inline SSeConnectionType type() const { return m_type; }
    void init();
    SSeConnectionType m_type;
    double m_v, m_initmin, m_initmax, m_eta;
};

#endif // SSCCONNECTION_HPP
