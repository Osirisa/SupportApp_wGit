#ifndef ENCRYPTIONHELPER_H
#define ENCRYPTIONHELPER_H

#include <QString>
#include <QByteArray>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

class EncryptionHelper {
public:
    EncryptionHelper();
    QByteArray encrypt(const QString& data, const QString& password);
    QString decrypt(const QByteArray& ciphertext, const QString& password);

private:
    static const int saltLength = 32; // Length of the salt in bytes
    static const int iterationCount = 10000; // Number of PBKDF2 iterations
    static const int keyLength = 32; // Length of the derived key in bytes
    bool generateSalt(unsigned char* salt);
    bool deriveKeyFromPassword(const QString& password, const unsigned char* salt, unsigned char* key);
};

#endif // ENCRYPTIONHELPER_H
