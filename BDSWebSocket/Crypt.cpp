#include "pch.h"
#include "Crypt.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/base64.h>
#include <cryptopp/md5.h>

using namespace std;
using namespace CryptoPP;

// https://github.com/WangYneos/BDXWebSocket/blob/master/example/bdxws/encrypt_helper.h

static const string base64_table =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64_char(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/') || (c == '='));
}
static inline bool is_base64(string str) {
	if (str.length() % 4 != 0) {
		return false;
	}
	for (int a = 0; a < str.length(); a++) {
		if (!is_base64_char(str[a]))
			return false;
	}
	return true;
}
string base64_encode(string const& ori) {
	char const* bytes_to_encode = ori.c_str();
	size_t length = ori.length();
	string final_base64;
	int i = 0;
	int j = 0;
	unsigned char char_3[3];
	unsigned char char_4[4];
	while (length--) {
		char_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_4[0] = (char_3[0] & 0xfc) >> 2;
			char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
			char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
			char_4[3] = char_3[2] & 0x3f;
			for (i = 0; (i < 4); i++)
				final_base64 += base64_table[char_4[i]];
			i = 0;
		}
	}
	if (i)
	{
		for (j = i; j < 3; j++)
			char_3[j] = '\0';
		char_4[0] = (char_3[0] & 0xfc) >> 2;
		char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
		char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
		char_4[3] = char_3[2] & 0x3f;
		for (j = 0; (j < i + 1); j++)
			final_base64 += base64_table[char_4[j]];
		while ((i++ < 3))
			final_base64 += '=';
	}
	return final_base64;
}

string base64_aes_cbc_encrypt(const string& ori, const AESKey& ak) {

	string encrypted;
	CryptoPP::AES::Encryption aesEncryption(ak.key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, ak.iv);
	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(encrypted));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(ori.c_str()), ori.length());
	stfEncryptor.MessageEnd();

	string base64;
	CryptoPP::Base64Encoder encoder(nullptr, false);
	encoder.Attach(new CryptoPP::StringSink(base64));
	encoder.Put(reinterpret_cast<const unsigned char*>(encrypted.c_str()), encrypted.length());
	encoder.MessageEnd();

	return base64;
}

string base64_aes_cbc_decrypt(const string& cipher, const AESKey& ak) {
	string decoded_base64;
	string decrypted;
	CryptoPP::Base64Decoder decoder;
	decoder.Attach(new CryptoPP::StringSink(decoded_base64));
	decoder.Put(reinterpret_cast<const unsigned char*>(cipher.c_str()), cipher.length());
	decoder.MessageEnd();

	CryptoPP::AES::Decryption aesDecryption(ak.key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, ak.iv);
	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(decoded_base64.c_str()), decoded_base64.size());
	stfDecryptor.MessageEnd();
	return decrypted;
}

inline string MD5(const string& src) {
	string buf;
	string opt;
	Weak::MD5 hash;
	HexEncoder encoder(new StringSink(opt));
	hash.Update((const CryptoPP::byte*)&src[0], src.size());
	buf.resize(hash.DigestSize());
	hash.Final((CryptoPP::byte*)&buf[0]);
	StringSource ss(buf, true, new Redirector(encoder));
	return opt;
}

string md5(const string& in) {
	string ori = MD5(in);
	std::transform(ori.begin(), ori.end(), ori.begin(), ::tolower);
	return ori;
}