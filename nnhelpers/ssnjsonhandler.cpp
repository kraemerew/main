#include "ssnjsonhandler.hpp"
#include <QJsonDocument>
#include <QJsonObject>

QByteArray SSnJsonHandler::toData(const QVariantMap &vm)
{
    QJsonDocument d = QJsonDocument::fromVariant(vm);
    return d.toJson();
}

QVariantMap SSnJsonHandler::fromData(const QByteArray &ba, bool &ok)
{
    ok = false;
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(ba,&e);
    if (e.error==QJsonParseError::NoError)
    {
        ok = true;
        return d.toVariant().toMap();
    }
    return QVariantMap();
}
