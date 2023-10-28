#include "encryptionhelper.h"

EncryptionHelper::EncryptionHelper(const QString& key)
    : key(key) {}

QString EncryptionHelper::encryptDecrypt(const QString& data)
{
    QString output(data.size(), Qt::Uninitialized);
    for(int i = 0; i < data.size(); i++) {
        output[i] = QChar(data[i].unicode() ^ key[i % key.size()].unicode());
    }
    return output;
}
