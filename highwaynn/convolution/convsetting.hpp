#ifndef CONVSETTING_HPP
#define CONVSETTING_HPP

#include <QVariantMap>
#include <QSize>

/*!
 * \brief Create/fit valid convolution settings for input layer and hidden layers
 *        In both cases kernel and kernel stride and either ouput size (for input layer case) or input size (for hidden layer) are needed
 */
class SScConvSetting
{
public:
    /*!
     * \brief Create input convolution setting based o kernel stride and output size
     * \param kernel    Kernel size
     * \param stride    Kernel stride
     * \param output    Output elements
     */
    explicit SScConvSetting(const QSize& kernel, const QSize& stride, const QSize& output);
    /*!
     * \brief Create hidden convolution setting based on a input convolution setting
     * \param kernel
     * \param stride
     * \param input
     */
    explicit SScConvSetting(const QSize& kernel, const QSize& stride, const SScConvSetting& input);
    /*!
     * \brief From serialized
     * \param vm
     */
    explicit SScConvSetting(const QVariantMap& vm);
    /*!
     * \brief Serialize
     * \return
     */
    QVariantMap toVm() const;

    static SScConvSetting fromVm(const QVariantMap& vm) { return SScConvSetting(vm); }

    inline bool     isValid     () const { return kernelValid() && strideValid() && inputValid() && outputValid(); }
    inline bool     kernelValid () const { return (m_k.width()>1) && (m_k.height()>1); }
    inline bool     strideValid () const { return (m_s.width()>0) && (m_s.height()>0); }
    inline bool     inputValid  () const { return (m_i.width()>0) && (m_i.height()>0); }
    inline bool     outputValid () const { return (m_o.width()>0) && (m_o.height()>0); }

           QSize    overlap     () const;
    inline QSize    kernel      () const { return m_k; }
    inline QSize    stride      () const { return m_s; }
    inline QSize    input       () const { return m_i; }
    inline QSize    output      () const { return m_o; }

    inline bool     canInputFrom(const SScConvSetting& other) const { return input()==other.output(); }
    inline bool     canOutputTo (const SScConvSetting& other) const { return output()==other.input(); }

private:
    bool m_inputtype;
    QSize m_k, m_s, m_o, m_i;
};

#endif // CONVSETTING_HPP
