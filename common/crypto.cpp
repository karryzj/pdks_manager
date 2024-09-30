#include "crypto.h"
#include <QFile>
#include <QDebug>

namespace cm
{
const static unsigned int aes128_block_size = 16;

QByteArray Crypto::generate_key(const QString &password, const QByteArray &read_salt)
{
    QByteArray passwd_tmp = password.toUtf8();
    QByteArray passwd_array = add_padding128(passwd_tmp);
    std::vector<unsigned char> vec_passwd(passwd_array.begin(), passwd_array.end());
    std::vector<unsigned char> vec_salt(read_salt.begin(), read_salt.end());
    std::vector<unsigned char> vec_ret;

    AES aes(AESKeyLength::AES_128);
    vec_ret = aes.EncryptCBC(vec_passwd, vec_salt, vec_salt);
    QByteArray key(reinterpret_cast<const char*>(vec_ret.data()), vec_ret.size());
    return key;
}

QByteArray Crypto::encrypt_byte_array(const QByteArray &input_original, const QString &password, const QByteArray &read_salt)
{
    // 从密码生成密钥
    QByteArray key = generate_key(password, read_salt);
    QByteArray input_array = add_padding128(input_original);
    std::vector<unsigned char> vec_input(input_array.begin(), input_array.end());
    std::vector<unsigned char> vec_salt(read_salt.begin(), read_salt.end());
    std::vector<unsigned char> vec_key(key.begin(), key.end());
    std::vector<unsigned char> vec_ret;

    // 使用 AES 进行加密
    AES aes(AESKeyLength::AES_128);
    vec_ret = aes.EncryptCBC(vec_input, vec_key, vec_salt);
    QByteArray encryptedData(reinterpret_cast<const char*>(vec_ret.data()), vec_ret.size());
    return encryptedData;
}

QByteArray Crypto::decrypt_byte_array(const QByteArray &input_encrypt, const QString &password, const QByteArray &read_salt)
{
    // 从密码生成密钥
    QByteArray key = generate_key(password, read_salt);
    QByteArray input_array = add_padding128(input_encrypt);
    std::vector<unsigned char> vec_input(input_array.begin(), input_array.end());
    std::vector<unsigned char> vec_salt(read_salt.begin(), read_salt.end());
    std::vector<unsigned char> vec_key(key.begin(), key.end());
    std::vector<unsigned char> vec_ret;

    // 使用 AES 进行解密
    AES aes(AESKeyLength::AES_128);
    vec_ret = aes.DecryptCBC(vec_input, vec_key, vec_salt);
    QByteArray decrypted_array(reinterpret_cast<const char*>(vec_ret.data()), vec_ret.size());
    QByteArray decrypted_data = remove_padding128(decrypted_array);
    return decrypted_data;
}

QString Crypto::encrypt_string(const QString &input_original, const QString &password, const QByteArray &read_salt)
{
    QString ret_str;
    QByteArray ret = encrypt_byte_array(input_original.toUtf8(), password, read_salt);
    if (!ret.isEmpty())
    {
        ret_str = ret;
    }
    return ret_str;
}

QString Crypto::decrypt_string(const QString &input_encrypt, const QString &password, const QByteArray &read_salt)
{
    QString ret_str;
    QByteArray ret = decrypt_byte_array(input_encrypt.toUtf8(), password, read_salt);
    if (!ret.isEmpty())
    {
        ret_str = ret;
    }
    return ret_str;
}

QString Crypto::decrypt_json_doc_to_string(const QString &input_file, const QString &password)
{
    // 读取加密的文件
    QFile inputFile(input_file);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open input file";
        return "";
    }

    // 读取盐值和 IV
    QByteArray read_salt = inputFile.read(16);  // 读取 16 字节的盐
    QByteArray encryptedData = inputFile.readAll(); // 读取加密数据
    inputFile.close();

    QByteArray ret = decrypt_byte_array(encryptedData, password, read_salt);
    if (!ret.isEmpty())
    {
        // 将解密后的数据转换回
        return QString(ret);
    }
    else
    {
        return "";
    }
}

void Crypto::encrypt_file(const QString &input_file, const QString &output_file, const QString &password)
{
    // 读取待加密的文件
    QFile inputFile(input_file);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open input file";
        return;
    }


    QByteArray oriData = inputFile.readAll(); // 读取加密数据
    inputFile.close();

    // 生成一个盐
    QByteArray read_salt;
    quint8 iv_16[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    for (int i=0; i<16; i++)
    {
        read_salt.append(iv_16[i]);
    }

    QByteArray ret = encrypt_byte_array(oriData, password, read_salt);
    if (!ret.isEmpty())
    {
        // 将盐、IV 和加密数据保存到文件
        QFile outputFile(output_file);
        if (outputFile.open(QIODevice::WriteOnly))
        {
            outputFile.write(read_salt);          // 保存盐
            outputFile.write(ret);     // 保存加密数据
            outputFile.close();
        }
    }
}

QByteArray Crypto::add_padding128(const QByteArray &input)
{
    QByteArray ret;
    if (input.size() % aes128_block_size)
    {
        ret = input;
        int num = aes128_block_size - input.size() % aes128_block_size;
        QByteArray pad(num, 0);
        ret.append(pad);
        return ret;
    }
    else
    {
        return input;
    }
}

QByteArray Crypto::remove_padding128(const QByteArray &input)
{
    QByteArray ret(input);

    //Works only if the last byte of the decoded array is not zero
    while (ret.at(ret.length()-1) == 0x00)
        ret.remove(ret.length()-1, 1);

    return ret;
}

void Crypto::encrypt_json_doc(const QJsonDocument &json_doc, const QString &output_file, const QString &password)
{
    // 将 QJsonDocument 转换为 QByteArray
    QByteArray jsonData = json_doc.toJson();

    // 生成一个盐
    QByteArray read_salt;
    quint8 iv_16[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    for (int i=0; i<16; i++)
    {
        read_salt.append(iv_16[i]);
    }
    QByteArray ret = encrypt_byte_array(jsonData, password, read_salt);
    if (!ret.isEmpty())
    {
        // 将盐、IV 和加密数据保存到文件
        QFile outputFile(output_file);
        if (outputFile.open(QIODevice::WriteOnly))
        {
            outputFile.write(read_salt);          // 保存盐
            outputFile.write(ret);     // 保存加密数据
            outputFile.close();
        }
    }
}

QJsonDocument Crypto::decrypt_json_doc(const QString &input_file, const QString &password)
{
    // 读取加密的文件
    QFile inputFile(input_file);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open input file";
        return QJsonDocument();
    }

    // 读取盐值和 IV
    QByteArray read_salt = inputFile.read(16);  // 读取 16 字节的盐
    QByteArray encryptedData = inputFile.readAll(); // 读取加密数据
    inputFile.close();

    QByteArray ret = decrypt_byte_array(encryptedData, password, read_salt);
    if (!ret.isEmpty())
    {
        // QFile outputFile("output_file.json");
        // if (outputFile.open(QIODevice::WriteOnly))
        // {
        //     outputFile.write(ret);     // 保存加密数据
        //     outputFile.close();
        // }
        // 将解密后的数据转换回 QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(ret);
        if (jsonDoc.isNull())
        {
            qWarning() << "Failed to parse JSON document";
        }

        return jsonDoc;
    }
    else
    {
        return QJsonDocument();
    }
}
}
