/*
  NAME
    AESCrypt.hpp - Header file of the AESCrypt class.

  DISCRIPTION
    Encrypt/Decrypt string
*/

#ifndef __AESCRYPT_HPP__
#define __AESCRYPT_HPP__

#include <openssl/aes.h>
#include <string>

class AESCrypt
{
  public:
    AESCrypt(const std::string& in_key);

    bool set_key(const std::string& in_key);
    std::string get_key();
    std::string gen_key();
    bool encrypt(const std::string& in_str, std::string& encry_str);
    bool decrypt(const std::string& in_str, std::string& decry_str);

  private:
    std::string aes_key;
    AES_KEY enc_key;
    AES_KEY dec_key;
    unsigned int char2hexstring(const unsigned char* str, std::string& out, unsigned int maxlen);
    unsigned int hexstring2char(const std::string& str, unsigned char* out, unsigned int maxlen);
};

#endif
