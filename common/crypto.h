#ifndef __CRYPTO_JSON_H
#define __CRYPTO_JSON_H

#include "AES.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "common.h"

namespace cm
{

class CM_PUBLIC Crypto
{
public:
    static QByteArray generate_key(const QString &password = "ququan123456", const QByteArray &salt = QByteArray(16,1));
    static void encrypt_json_doc(const QJsonDocument &json_doc, const QString &output_file, const QString &password = "ququan123456");
    static QJsonDocument decrypt_json_doc(const QString &input_file, const QString &password = "ququan123456");
    static QByteArray encrypt_byte_array(const QByteArray &input_original, const QString &password = "ququan123456", const QByteArray &read_salt = QByteArray(16,1));
    static QByteArray decrypt_byte_array(const QByteArray &input_encrypt, const QString &password = "ququan123456", const QByteArray &read_salt = QByteArray(16,1));
    static QString encrypt_string(const QString &input_original, const QString &password = "ququan123456", const QByteArray &read_salt = QByteArray(16,1));
    static QString decrypt_string(const QString &input_encrypt, const QString &password = "ququan123456", const QByteArray &read_salt = QByteArray(16,1));
    static QString decrypt_json_doc_to_string(const QString &input_file, const QString &password= "ququan123456");
    static void encrypt_file(const QString &input_file, const QString &output_file, const QString &password= "ququan123456");

private:
    static QByteArray add_padding128(const QByteArray &input);
    static QByteArray remove_padding128(const QByteArray &input);
};
}

#endif // __CRYPTO_JSON_H
