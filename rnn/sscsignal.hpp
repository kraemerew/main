#ifndef SSCSIGNAL_HPP
#define SSCSIGNAL_HPP

#include <QVector>
#include <QMap>
#include <QHash>

template <typename T> class SScSignal
{
public:
    /*!
     * \brief Return default element if index does not exist (no elements created)
     * \param t
     * \return
     */
    virtual  T operator [] (int t) const = 0;
    /*!
     * \brief create and return element
     * \param t
     * \return
     */
    virtual T& operator[] (int t) = 0;
    /*!
     * \brief Guaranteed to be const / don't add elements
     * \param t
     * \return
     */
    virtual T at(int t) const { return (*this)[t]; }
    /*!
     * \brief contains
     * \param t
     * \return
     */
    virtual bool contains(int t) const = 0;
    /*!
     * \brief Clear all data
     */
    virtual void clear() = 0;
};

/*!
 *  Vector based signal - index below 0 will return 0
 */
template <typename T> class SScStdSignal : public SScSignal<T>
{
public:
    explicit SScStdSignal() : SScSignal<T>(), dummy(0) { }
    virtual T operator [] (int t) const { return (t<0) || (t>=v.size()) ? 0 : v[t]; }
    virtual T& operator[] (int t)
    {
        if (t<0) return dummy;
        if (!contains(t))
        {
            QVector<T> v2;
            v2.fill(0,t-v.size()+1);
            v.append(v2);
        }
        return v[t];
    }
    virtual bool contains(int t) const { return (t>=0) && (t<v.size()); }
    virtual void clear() { v.clear(); }

private:
    T dummy;
    QVector<T> v;
};

/*!
 * Map based signal - sparse indexing, elements not set will return 0
 */
template <typename T> class SScMapSignal : public SScSignal<T>
{
public:
    explicit SScMapSignal() : SScSignal<T>() {}
    virtual T operator [] (int t) const { return  (m.contains(t)) ? m[t] : 0; }
    virtual T& operator[] (int t) { return m[t]; }
    virtual bool contains(int t) const { return m.contains(t); }
    virtual void clear() { m.clear(); }

private:
    QMap<int,T> m;
};

/*!
 * Hash based signal - same as map, cant be sorted (easily) with better time behavior
 */
template <typename T> class SScHashSignal : public SScSignal<T>
{
public:
    explicit SScHashSignal() : SScSignal<T>() {}
    virtual T operator [] (int t) const { return (h.contains(t)) ? h[t] : 0; }
    virtual T& operator[] (int t) { return h[t]; }
    virtual bool contains(int t) const { return h.contains(t); }
    virtual void clear() { h.clear(); }

private:
    QHash<int,T> h;
};
#endif // SSCSIGNAL_HPP
