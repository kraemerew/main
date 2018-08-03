#include "sscvariantmap.hpp"
#include <QJsonObject>
#include <QJsonDocument>

void SScVariantMap::operator = (const QVariantMap& other) { (*this)=other; }

QByteArray SScVariantMap::toJson() const
{
    QJsonObject obj = QJsonObject::fromVariantMap(*this);
    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

SScVariantMap SScVariantMap::fromJson(const QByteArray& data)
{
    SScVariantMap retValue;
    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(data,&pe);
    if (pe.error==QJsonParseError::NoError)
    {
        retValue = doc.object().toVariantMap();
    }
    return retValue;
}
