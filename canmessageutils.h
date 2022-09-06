#ifndef CANMESSAGEUTILS_H
#define CANMESSAGEUTILS_H

#include <QHash>
#include <QString>

class CanMessageUtils
{
public:
    struct Field
    {
        QString name;
        bool isSigned;
        bool isBigEndian;
        int bitpos;
        int length;
        double factor;
        double offset;
        double minimum;
        double maximum;
        QString unit;
    };

    using Fields = QHash<QString, Field>;

    static Field parseField(const QString& field);
    static Fields parseFields(const QString& field);

    double readField(const QByteArray& data, const Field& field);
};

#endif
