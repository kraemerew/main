#include "convsetting.hpp"
#include "sscvm.hpp"
#include "convhelpers.hpp"

SScConvSetting::SScConvSetting(const QSize &kernel, const QSize &stride, const QSize &output)
    : m_inputtype(false),
      m_k(kernel),
      m_s(stride),
      m_o(output)
{
    if (kernelValid() && strideValid() && outputValid())
        m_i = SSnConvHelper::inputSize(m_k,m_s,m_o);
}

SScConvSetting::SScConvSetting(const QSize &kernel, const QSize &stride, const SScConvSetting &input)

    : m_inputtype(true),
      m_k(kernel),
      m_s(stride),
      m_o(QSize(-1,-1)),
      m_i(input.input())
{
    if (kernelValid() && strideValid() && inputValid())
        m_o = SSnConvHelper::fitToInput(m_i,m_k,m_s);
}


SScConvSetting::SScConvSetting(const QVariantMap &vm)
{
    SScVM sscvm(vm);
    m_inputtype = sscvm.boolToken("INPUTLAYER",true);
    m_k = QSize(sscvm.intToken("KERNEL_X"), sscvm.intToken("KERNEL_Y"));
    m_s = QSize(sscvm.intToken("STRIDE_X"), sscvm.intToken("STRIDE_Y"));
    m_o = QSize(sscvm.intToken("OUTPUT_X"), sscvm.intToken("OUTPUT_Y"));
    m_i = QSize(sscvm.intToken("INPUT_X"),  sscvm.intToken("INPUT_Y"));
}


QVariantMap SScConvSetting::toVm() const
{
    QVariantMap ret;
    ret["INPUTLAYER"] = m_inputtype;
    ret["KERNEL_X"] = m_k.width(); ret["KERNEL_Y"] = m_k.height();
    ret["STRIDE_X"] = m_s.width(); ret["STRIDE_Y"] = m_s.height();
    ret["OUTPUT_X"] = m_o.width(); ret["OUTPUT_Y"] = m_o.height();
    ret["INPUT_X"]  = m_i.width(); ret["INPUT_Y"]  = m_i.height();
    return ret;
}

QSize    SScConvSetting::overlap     () const { return SSnConvHelper::stride2Overlap(kernel(),stride()); }
