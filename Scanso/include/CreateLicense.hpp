#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <QFile>
#include <string>

inline int create_license(const std::string& _od, const std::string& _path)
{
    // 使用 QFile 读取资源文件
    QFile priv_key_file{":/pem/private.pem"};
    if (!priv_key_file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    // 将文件内容读取到内存
    QByteArray priv_key_data = priv_key_file.readAll();
    priv_key_file.close();
    // 使用 BIO 读取私钥
    BIO* bio = BIO_new_mem_buf(priv_key_data.data(), priv_key_data.size());
    if (!bio)
    {
        return -1;
    }
    RSA* rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (!rsa)
    {
        return -1;
    }
    // 生成签名
    unsigned char sig[256]{};
    unsigned int  sig_len{};
    if (RSA_sign(NID_sha256, (unsigned char*)_od.data(), _od.size(), sig, &sig_len, rsa) != 1)
    {
        RSA_free(rsa);
        return -1;
    }
    // 写入签名文件
    FILE* license_file = fopen(_path.c_str(), "wb");
    if (!license_file)
    {
        RSA_free(rsa);
        return -1;
    }
    fwrite(sig, 1, sig_len, license_file);
    fclose(license_file);
    RSA_free(rsa);
    return 0;
}