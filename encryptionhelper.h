// encryptionhelper.h
#ifndef ENCRYPTIONHELPER_H
#define ENCRYPTIONHELPER_H

#include <QString>

class EncryptionHelper
{
public:
    EncryptionHelper(const QString& key);
    QString encryptDecrypt(const QString& data);

private:
    QString key;
};

#endif // ENCRYPTIONHELPER_H
