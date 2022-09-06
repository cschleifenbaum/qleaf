#include "canmessageutils.h"

#include <QDebug>
#include <QRegularExpression>

#include <QtEndian>

CanMessageUtils::Fields CanMessageUtils::parseFields(const QString& fields)
{
    Fields result;

    for (const auto& line : fields.split(QRegularExpression(QStringLiteral("\n|\r\n|\r"))))
    {
        auto f = parseField(line);
        if (!f.name.isEmpty())
            result.insert(f.name, f);
    }

    return result;
}

CanMessageUtils::Field CanMessageUtils::parseField(const QString& field)
{
    // "SG_ MaximumBatteryVoltage : 39|16@0+ (1,0) [0|600] "V" Vector__XXX"
   
    static QRegularExpression re(QStringLiteral("^([^ ]+) +([^: ]+).* *: *(\\d*)\\|(\\d*)@(\\d)([+-]) *\\(([\\d.]*),([\\d.]*)\\) *\\[([^|]*)\\|([^\\]]*)\\] *\\\"([^\\\"]*)\\\""));

    auto match = re.match(field);
    if (match.hasMatch())
    {
        Field f;
//        qDebug() << match.captured(1); // "SG_"
        f.name = match.captured(2);                            // "MaximumBatteryVoltage"
        f.bitpos = match.captured(3).toInt();                  // "39"
        f.length = match.captured(4).toInt();                  // "16"
        f.isBigEndian = match.captured(5).toInt() == 0       ; // "0" (0 Motorola, 1 Intel)
        f.isSigned = match.captured(6) == QStringLiteral("-"); // "+" (+ unsigned, - signed)
        f.factor = match.captured(7).toDouble();               // "1"
        f.offset = match.captured(8).toDouble();               // "0"
        f.minimum = match.captured(9).toDouble();              // "0"
        f.maximum = match.captured(10).toDouble();             // "600"
        f.unit = match.captured(11);                           // "V"

        if (f.isBigEndian)
        {
            // bitpos = 39
            int byte = f.bitpos / 8; // 4 
            int localbitpos = f.bitpos % 8; // 7
            localbitpos = -localbitpos + 7; // 0
            int newbitpos = byte * 8 + localbitpos; // 32
            newbitpos += f.length - 1; // 47
            byte = newbitpos / 8; // 5
            localbitpos = newbitpos % 8; // 7
            localbitpos = -localbitpos + 7; // 0
            newbitpos = byte * 8 + localbitpos; // 40
            f.bitpos = newbitpos;
        }

        return f;
    }

    qDebug() << "Cannot parse" << field;

    return {};
}

double CanMessageUtils::readField(const QByteArray& data, const Field& field)
{
    int result = 0;

    int bitsread = 0;
    int bitpos = field.bitpos;
    int length = field.length;

    while (length > 0) {
        int byte = bitpos / 8;
        int localbitpos = bitpos % 8;
        int bitstoread = std::min(length, 8 - localbitpos);
        uchar value = data[byte];
        uchar mask = uchar(0xff << 8 >> bitstoread) >> (8 - bitstoread) << localbitpos;
        int shift = localbitpos - bitsread;
        result += (int(value & mask) >> std::max(0, shift) << std::max(0, -shift));

        bitsread += bitstoread;
        length -= bitstoread;
        bitpos += bitstoread - 16;
    }

    if (field.isBigEndian)
    {
        if (field.length == 16)
        {
            result = qFromBigEndian<quint16>(result);
        }
        else
        {
            qDebug() << "no idea how to fix endianess..." << field.name;
        }
    }

    if (field.isSigned)
    {
        bool msbit = result & (1 << (bitsread - 1));
        if (msbit) {
            for (int i = bitsread; i < 32; ++i)
                result |= (1 << i);
        }
    }

    return result * field.factor + field.offset;
}

