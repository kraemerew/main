#ifndef SSNJSONHANDLER_HPP
#define SSNJSONHANDLER_HPP

#include <QVariantMap>
#include <QByteArray>

namespace SSnJsonHandler
{
QByteArray toData(const QVariantMap& vm);
QVariantMap fromData(const QByteArray& ba, bool& ok);
}

#endif // SSNJSONHANDLER_HPP
