#ifndef CRYPT_H
#define CRYPT_H
#include <string>

struct AESKey {
	unsigned char key[20], iv[20];
};

std::string base64_aes_cbc_encrypt(const std::string& plain, const AESKey& ak);

std::string base64_aes_cbc_decrypt(const std::string& cipher, const AESKey& ak);

std::string md5(const std::string& in);

#endif // !CRYPT_H

