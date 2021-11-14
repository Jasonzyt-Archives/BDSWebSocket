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
#include <cryptopp/pkcspad.h>

using namespace std;
using namespace CryptoPP;

string base64_aes_cbc_encrypt(const string& plain, const AESKey& ak) {
	string encrypted;
	CryptoPP::AES::Encryption aesEncryption(ak.key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, ak.iv);
	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, 
		new CryptoPP::StringSink(encrypted));
	stfEncryptor.Put((const CryptoPP::byte*)&plain[0], plain.size());
	stfEncryptor.MessageEnd();

	string base64;
	CryptoPP::Base64Encoder encoder(nullptr, false);
	encoder.Attach(new CryptoPP::StringSink(base64));
	encoder.Put((const CryptoPP::byte*)&encrypted[0], encrypted.size());
	encoder.MessageEnd();
	return base64;
}

string base64_aes_cbc_decrypt(const string& cipher, const AESKey& ak) {
	string encrypted;
	CryptoPP::Base64Decoder decoder;
	decoder.Attach(new CryptoPP::StringSink(encrypted));
	decoder.Put((const CryptoPP::byte*)&cipher[0], cipher.size());
	decoder.MessageEnd();

	string decrypted;
	CryptoPP::AES::Decryption aesDecryption(ak.key, CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, ak.iv);
	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, 
		new CryptoPP::StringSink(decrypted));
	stfDecryptor.Put((const CryptoPP::byte*)&encrypted[0], encrypted.size());
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