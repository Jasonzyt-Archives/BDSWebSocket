#ifndef CRYPT_H
#define CRYPT_H
#include <string>

std::string base64_encode(std::string const& ori);

std::string base64_aes_cbc_encrypt(const std::string& ori,
	unsigned char aes_key[], unsigned char aes_iv[]);

std::string base64_aes_cbc_decrypt(const std::string& cipher, 
	unsigned char aes_key[], unsigned char aes_iv[]);

std::string md5(const std::string& in);

#endif // !CRYPT_H

