#ifndef CRYPT_H
#define CRYPT_H
#include <string>

struct AESKey {
	unsigned char key[16], iv[16];
};

std::string base64_encode(std::string const& ori);

std::string base64_aes_cbc_encrypt(const std::string& ori, const AESKey& ak);

std::string base64_aes_cbc_decrypt(const std::string& cipher, const AESKey& ak);

std::string md5(const std::string& in);

#endif // !CRYPT_H

