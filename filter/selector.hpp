#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <QList>
#include <QtMath>

template <typename T> class SScSelector : public QList<T>
{
public:
    explicit SScSelector() : m_sorted(false) {}
    inline  double get(double sel)
    {
        if (this->isEmpty()) return 0;
        std::sort(this->begin(),this->end());
        if (!m_sorted) std::sort(this->begin(),this->end());
        const double d = qBound(0.0,sel,1.0)*(this->size()-1);
        const int s0 = qFloor(d), s1 = qCeil(d);
        const double f0 = 1.0-(d-(double)s0), f1 = 1.0-f0;
        return f0*(double)this->at(s0)+f1*(double)this->at(s1);
    }

private:
    bool    m_sorted;
};

#endif // SELECTOR_HPP
