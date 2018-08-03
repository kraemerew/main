#ifndef SSC_VARIANTMAP_HPP
#define SSC_VARIANTMAP_HPP
#include <QVariantMap>

class SScVariantMap : public QVariantMap
{
public:
QByteArray toJson() const;
static SScVariantMap fromJson(const QByteArray& data);
void operator = (const QVariantMap&);
};

#endif // SSC_VARIANTMAP_HPP
