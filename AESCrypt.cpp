/*
   NAME
     AESCrypt.cpp - Body file of the AESCrypt class

   DESCRIPTION
     Encrypt/Decrypt string
     AES加/解密
*/

#include "AESCrypt.hpp"
#include "Symbol.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstring>

//构造函数
AESCrypt::AESCrypt(const std::string& in_key)
{
  set_key(in_key);
}

//设置当前加密、解密的密钥
bool AESCrypt::set_key(const std::string& in_key)
{
  if(in_key.size() < AES_BLOCK_SIZE * 2) {
    return false;
  }
  // which will faster?
  // std::string keystr = in_key.substr(0, AES_BLOCK_SIZE * 2);
  std::string keystr(in_key, 0, AES_BLOCK_SIZE * 2);
  if(aes_key == keystr) {
    return true;
  }

  unsigned char keyarr[AES_BLOCK_SIZE];
  memset(keyarr, 0x00, sizeof(keyarr));
  if(AES_BLOCK_SIZE != hexstring2char(keystr, keyarr, AES_BLOCK_SIZE)) {
    return false;
  }

  if(AES_set_encrypt_key(keyarr, AES_BLOCK_SIZE * 8, &enc_key) < 0) {
    return false;
  }
  if(AES_set_decrypt_key(keyarr, AES_BLOCK_SIZE * 8, &dec_key) < 0) {
    return false;
  }
  aes_key = keystr;

  return true;
}

//获取当前密钥
std::string AESCrypt::get_key()
{
  return aes_key;
}

//生成随机密钥
std::string AESCrypt::gen_key()
{
  srand(time(NULL));

  int i;
  unsigned char keyarr[AES_BLOCK_SIZE + 1];
  for(i = 0; i < AES_BLOCK_SIZE; i++) {
    keyarr[i] = rand() % 128;
  }
  keyarr[i] = 0x00;

  std::string key;
  char2hexstring(keyarr, key, AES_BLOCK_SIZE);
  return key;
}

//加密
bool AESCrypt::encrypt(const std::string& in_str, std::string& encry_str)
{
  bool result = false;
  if(aes_key.empty()) {
    return false;
  }

  unsigned char iv[AES_BLOCK_SIZE];
  memset(iv, 0x00, sizeof(iv));

  unsigned int length;
  if(0 == (in_str.size() % AES_BLOCK_SIZE)) {
    length = in_str.size();
  } else {
    length = (in_str.size() / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
  }
  const unsigned int len = length;
  const unsigned int len_tail = len + 1;

  unsigned char* strout = new unsigned char[len_tail];
  unsigned char* strin  = new unsigned char[len_tail];
  if(strout && strin) {
    memset(strout, 0x00, len_tail);
    memset(strin, 0x00, len_tail);
    strncpy((char*)strin, in_str.c_str(), len);
    AES_cbc_encrypt(strin, strout, len, &enc_key, iv, AES_ENCRYPT);
    char2hexstring(strout, encry_str, len);
    result = true;
  }
  if(strin) {
    delete[] strin;
  }
  if(strout) {
    delete[] strout;
  }

  return result;
}

//解密
bool AESCrypt::decrypt(const std::string& in_str, std::string& decry_str)
{
  bool result = false;
  if(aes_key.empty()) {
    return false;
  }

  unsigned char iv[AES_BLOCK_SIZE];
  memset(iv, 0x00, sizeof(iv));

  const unsigned int len = in_str.size();
  if(0 != (len % (2 * AES_BLOCK_SIZE))) {
    return false;
  }
  const unsigned int len_half = len / 2;
  const unsigned int len_half_tail = len_half + 1;

  unsigned char* strout = new unsigned char[len_half_tail];
  unsigned char* strin  = new unsigned char[len_half_tail];
  if(strout && strin) {
    memset(strout, 0x00, len_half_tail);
    memset(strin, 0x00, len_half_tail);
    if(len_half == hexstring2char(in_str, strin, len_half)) {
      AES_cbc_encrypt(strin, strout, len_half, &dec_key, iv, AES_DECRYPT);
      decry_str.assign((char*) strout);
    }
    result = true;
  }
  if(strin) {
    delete[] strin;
  }
  if(strout) {
    delete[] strout;
  }

  return result;
}

//字符串转换成十六进制字符串
unsigned int AESCrypt::char2hexstring(const unsigned char* str, std::string& out, unsigned int maxlen)
{
  std::ostringstream oss;
  oss << std::hex;
  oss << std::setfill(C_ZERO);
  oss << std::uppercase;   //大写
  unsigned int len = 0;
  while(str && len < maxlen) {
    oss << std::setw(2) << (unsigned int) *str++;
    len++;
  }
  out = oss.str();
  return len;
}

//char* out输出结果，n返回数据的长度
unsigned int AESCrypt::hexstring2char(const std::string& str, unsigned char* out, unsigned int maxlen)
{
  unsigned int n = 0;
  int temp = -1;
  std::stringstream ss;

  while(n < str.size() / 2 && n < maxlen) {
    ss << str.substr(n * 2, 2);
    ss >> std::hex;
    ss >> temp;
    if(temp < 0 || temp > 256) {
      return (-1);
    }
    out[n++] = temp;
    ss.str("");
    ss.clear();
    temp = -1;
  }

  return n;
}
