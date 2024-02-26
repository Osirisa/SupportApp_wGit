#include "encryptionhelper.h"
#include <openssl/evp.h>
#include <cstring>

EncryptionHelper::EncryptionHelper() {}

bool EncryptionHelper::generateSalt(unsigned char* salt) {
    return RAND_bytes(salt, saltLength) == 1;
}

bool EncryptionHelper::deriveKeyFromPassword(const QString& password, const unsigned char* salt, unsigned char* key) {
    return PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.length(),
                             salt, saltLength, iterationCount, EVP_sha256(), keyLength, key) == 1;
}

QByteArray EncryptionHelper::encrypt(const QString& data, const QString& password) {
    unsigned char salt[saltLength];
    if (!generateSalt(salt)) {
        return QByteArray(); // Failed to generate salt
    }

    unsigned char key[keyLength];
    if (!deriveKeyFromPassword(password, salt, key)) {
        return QByteArray(); // Failed to derive key
    }

    // Prepare for encryption
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, salt); // Using salt as IV for simplicity, consider using a separate IV

    // Encrypt
    QByteArray plaintext = data.toUtf8();
    int outlen;
    QByteArray ciphertext(plaintext.length() + EVP_MAX_BLOCK_LENGTH, 0);
    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &outlen, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.length());
    int tmplen;
    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + outlen, &tmplen);
    outlen += tmplen;

    EVP_CIPHER_CTX_free(ctx);

    // Prepend salt to ciphertext for use in decryption
    QByteArray result = QByteArray(reinterpret_cast<char*>(salt), saltLength) + ciphertext.left(outlen);
    return result;
}

QString EncryptionHelper::decrypt(const QByteArray& ciphertext, const QString& password) {
    if (ciphertext.length() <= saltLength) {
        return QString(); // Invalid ciphertext
    }

    // Extract salt from ciphertext
    unsigned char salt[saltLength];
    std::memcpy(salt, ciphertext.constData(), saltLength);

    unsigned char key[keyLength];
    if (!deriveKeyFromPassword(password, salt, key)) {
        return QString(); // Failed to derive key
    }

    // Prepare for decryption
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, salt); // Using salt as IV for simplicity

    // Decrypt
    QByteArray encryptedText = ciphertext.mid(saltLength);
    QByteArray plaintext(encryptedText.length(), 0);
    int outlen;
    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &outlen, reinterpret_cast<const unsigned char*>(encryptedText.data()), encryptedText.length());
    int tmplen;
    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + outlen, &tmplen);
    outlen += tmplen;

    EVP_CIPHER_CTX_free(ctx);

    return QString::fromUtf8(plaintext.left(outlen));
}
