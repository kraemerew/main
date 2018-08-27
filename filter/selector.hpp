#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <QList>
#include <QtMath>

template <typename T> class SScSelectorBase : public QList<T>
{
public:
    explicit SScSelectorBase() {}
    virtual T get() = 0;
};

template <typename T> class SScSelector : public SScSelectorBase<T>
{
public:
    explicit SScSelector(double sel) : SScSelectorBase<T>(), m_sorted(false), m_sel(qBound(0.0,sel,1.0)) {}
    virtual  T get()
    {
        if (this->isEmpty()) return 0;
        std::sort(this->begin(),this->end());
        if (!m_sorted) std::sort(this->begin(),this->end());
        const double d = qBound(0.0,m_sel,1.0)*(this->size()-1);
        const int s0 = qFloor(d), s1 = qCeil(d);
        const double f0 = 1.0-(d-(double)s0), f1 = 1.0-f0;
        return f0*(double)this->at(s0)+f1*(double)this->at(s1);
    }

private:
    bool    m_sorted;
    double  m_sel;
};

template <typename T> class SScMinSelector : public SScSelectorBase<T>
{
public:
    explicit SScMinSelector() : SScSelectorBase<T>() {}
    virtual  T get()
    {
        if (this->isEmpty()) return 0;
        T ret = this->first();
        foreach(const T& v, (*this)) if (v<ret) ret = v;
        return ret;
    }
};

template <typename T> class SScMaxSelector : public SScSelectorBase<T>
{
public:
    explicit SScMaxSelector() : SScSelectorBase<T>() {}
    virtual  T get()
    {
        if (this->isEmpty()) return 0;
        T ret = this->first();
        foreach(const T& v, (*this)) if (v>ret) ret = v;
        return ret;
    }

};

#endif // SELECTOR_HPP
