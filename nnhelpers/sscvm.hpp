#ifndef SSCVM_HPP
#define SSCVM_HPP

#include <QVariantMap>
#include <QDateTime>

class SScVM : public QVariantMap
{
public:
    explicit SScVM(const QVariantMap& vm = QVariantMap()) : QVariantMap(vm) {}
    bool            hasToken    (const QString& t, bool norm = true) const { return norm ? contains(normToken(t)) : contains(t); }
    QVariant        token       (const QString& t, bool norm = true) const { const QString t2 = norm ? normToken(t):t; return contains(t2) ? (*this)[t2] : QVariant(); }
    bool            boolToken   (const QString& t, bool                 dval = false,           bool norm = true) const { const QVariant v = token(t,norm); return v.canConvert<bool>() ? v.toBool() : dval; }
    int             intToken    (const QString& t, int                  dval = 0,               bool norm = true) const { const QVariant v = token(t,norm); return v.canConvert<int>() ? v.toInt() : dval; }
    double          doubleToken (const QString& t, double               dval = 0.0,             bool norm = true) const { const QVariant v = token(t,norm); return v.canConvert<double>() ? v.toDouble() : dval; }
    QDateTime       dttToken    (const QString& t, const QDateTime&     dval = QDateTime(),     bool norm = true) const { const QVariant v = token(t,norm); return (v.type()==QVariant::DateTime) ? v.toDateTime() : dval; }
    QString         stringToken (const QString& t, const QString&       dval = QString(),       bool norm = true) const { const QVariant v = token(t,norm); return v.isNull() ? dval : v.toString(); }
    QVariantMap     vmToken     (const QString& t, const QVariantMap&   dval = QVariantMap(),   bool norm = true) const { const QVariant v = token(t,norm); return (v.type()==QVariant::Map) ? v.toMap() : dval; }
    QVariantList    vlToken     (const QString& t, const QVariantList&  dval = QVariantList(),  bool norm = true) const { const QVariant v = token(t,norm); return (v.type()==QVariant::List) ? v.toList() : dval; }

private:
    QString normToken(const QString& token) const { return token.simplified().toUpper().replace(" ","_"); }
};

#endif // SSCVM_HPP
