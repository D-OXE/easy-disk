#include "../include/tools.h"
#include <cstdint>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>

const int AES_KEYLEN = 16; // AES-128
/**
 * @brief 实现4个加解密的函数, 编译时候需要增加-lcrypto选项
 *
 * @details 密文的前 16 字节是 IV，剩余部分是真正的加密数据。简单的 AES-256-CBC 加密（商业项目用 GCM）
 * 另外,key应该怎样保管?使用例子在最下面,:
    // 加密
    std::string ciphertext = encode_string(plaintext, key);

    // 解密
    std::string decrypted = decode_string(ciphertext, key);

    int ciphertext_len = encode_data(plaintext_buff, plaintext_len, ciphertext_buff, sizeof(ciphertext_buff), key);

	int decrypted_len = decode_data(ciphertext_buff, ciphertext_len, decrypted_buff, sizeof(decrypted_buff), key);
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 12:30:42
 */

// 短消息加密
std::string encode_string(const std::string& plaintext, const std::string& key)
{
    uint8_t iv[AES_KEYLEN];
    RAND_bytes(iv, AES_KEYLEN); // 随机生成 IV

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string ciphertext;

    std::vector<uint8_t> outbuf(plaintext.size() + AES_KEYLEN);

    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const uint8_t*>(key.data()), iv);

    int outlen = 0;
    EVP_EncryptUpdate(ctx, outbuf.data(), &outlen, reinterpret_cast<const uint8_t*>(plaintext.data()), plaintext.size());
    int total_len = outlen;

    EVP_EncryptFinal_ex(ctx, outbuf.data() + outlen, &outlen);
    total_len += outlen;

    EVP_CIPHER_CTX_free(ctx);

    // 把 IV 放在前面，拼接密文
    ciphertext.assign(reinterpret_cast<char*>(iv), AES_KEYLEN);
    ciphertext.append(reinterpret_cast<char*>(outbuf.data()), total_len);

    return ciphertext;
}

// 短消息传输解密
std::string decode_string(const std::string& ciphertext, const std::string& key)
{
    if (ciphertext.size() <= AES_KEYLEN)
	return {};

    const uint8_t* iv = reinterpret_cast<const uint8_t*>(ciphertext.data());
    const uint8_t* data = reinterpret_cast<const uint8_t*>(ciphertext.data() + AES_KEYLEN);
    size_t data_len = ciphertext.size() - AES_KEYLEN;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::string plaintext;
    std::vector<uint8_t> outbuf(data_len);

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const uint8_t*>(key.data()), iv);

    int outlen = 0;
    EVP_DecryptUpdate(ctx, outbuf.data(), &outlen, data, data_len);
    int total_len = outlen;

    EVP_DecryptFinal_ex(ctx, outbuf.data() + outlen, &outlen);
    total_len += outlen;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.assign(reinterpret_cast<char*>(outbuf.data()), total_len);
    return plaintext;
}

/**
 * @brief 传输的参数,缓冲区,长度,  	-->但是输出的缓冲区,我预计是8kb一次,那么output_max_len 应该最大为8192+16+32.
 * 										因为密文有个iv,16字节长, 而且加密有可能需要填充块,这个块EVP_MAX_BLOCK_LENGTH = 32
 * //TODO
 * @param input
 * @param input_len
 * @param output
 * @param output_max_len
 * @param key
 * @return int
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 21:08:06
 */
int encode_data(const uint8_t* input,  // 输入数据（明文）
		size_t input_len,      // 输入数据长度
		uint8_t* output,       // 输出缓冲区（存放密文）
		size_t output_max_len, // 输出缓冲区最大容量
		const std::string& key // 16字节密钥
)
{
    if (!input || !output || key.size() != AES_KEYLEN || output_max_len < input_len + AES_KEYLEN + EVP_MAX_BLOCK_LENGTH)
	{
	    return -1; // 参数检查失败
	}

    // 生成随机 IV
    uint8_t iv[AES_KEYLEN];
    if (RAND_bytes(iv, AES_KEYLEN) != 1)
	{
	    return -1;
	}

    // 将 IV 拷贝到输出头部
    std::memcpy(output, iv, AES_KEYLEN);

    // 加密
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int outlen = 0;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const uint8_t*>(key.data()), iv) != 1 ||
	EVP_EncryptUpdate(ctx, output + AES_KEYLEN, &outlen, input, input_len) != 1)
	{
	    EVP_CIPHER_CTX_free(ctx);
	    return -1;
	}
    int total_len = outlen;

    // 处理填充
    if (EVP_EncryptFinal_ex(ctx, output + AES_KEYLEN + outlen, &outlen) != 1)
	{
	    EVP_CIPHER_CTX_free(ctx);
	    return -1;
	}
    total_len += outlen;
    EVP_CIPHER_CTX_free(ctx);

    return AES_KEYLEN + total_len; // 返回总长度（IV + 密文）
}
// 解密函数：返回实际输出的明文长度（失败返回 -1）
int decode_data(const uint8_t* input,  // 输入数据（密文，含IV）
		size_t input_len,      // 输入数据长度
		uint8_t* output,       // 输出缓冲区（存放明文）
		size_t output_max_len, // 输出缓冲区最大容量
		const std::string& key // 16字节密钥
)
{
    if (!input || !output || key.size() != AES_KEYLEN || input_len < AES_KEYLEN || output_max_len < input_len - AES_KEYLEN)
	{
	    return -1; // 参数检查失败
	}

    // 提取 IV
    const uint8_t* iv = input;
    const uint8_t* encrypted_data = input + AES_KEYLEN;
    size_t encrypted_data_len = input_len - AES_KEYLEN;

    // 解密
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int outlen = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const uint8_t*>(key.data()), iv) != 1 ||
	EVP_DecryptUpdate(ctx, output, &outlen, encrypted_data, encrypted_data_len) != 1)
	{
	    EVP_CIPHER_CTX_free(ctx);
	    return -1;
	}
    int total_len = outlen;

    // 处理填充
    if (EVP_DecryptFinal_ex(ctx, output + outlen, &outlen) != 1)
	{
	    EVP_CIPHER_CTX_free(ctx);
	    return -1;
	}
    total_len += outlen;
    EVP_CIPHER_CTX_free(ctx);

    return total_len; // 返回明文长度
}
/*
//简单的测试string加解密.----> ok
int main()
{
    std::string key = "0123456789abcdef"; // 16字节密钥
    std::string plaintext = "Hello, World!";

    // 加密
    std::string ciphertext = encode_string(plaintext, key);

    // 解密
    std::string decrypted = decode_string(ciphertext, key);

    std::cout << "Decrypted: " << decrypted << std::endl; // 输出 "Hello, World!"
    return 0;
}
*/

/*简单的测试 二进制数 加解密.----> ok
int main()
{
    std::string key = "0123456789abcdef";														 // 16字节密钥
    uint8_t plaintext_buff[4096] = "这个加解密主要是用于文件传输,这里会有大文件拆成4096一个片,加密然后传输,当文件到达末尾时候,可能4096的缓冲区未必填满"; // 明文数据
    size_t plaintext_len = strlen(reinterpret_cast<char*>(plaintext_buff));

    // 加密
    uint8_t ciphertext_buff[4096]; // 密文缓冲区
    int ciphertext_len = encode_data(plaintext_buff, plaintext_len, ciphertext_buff, sizeof(ciphertext_buff), key);
    if (ciphertext_len == -1)
	{
	    std::cerr << "Encryption failed!" << std::endl;
	    return 1;
	}

    // 解密
    uint8_t decrypted_buff[4096]; // 明文缓冲区
    int decrypted_len = decode_data(ciphertext_buff, ciphertext_len, decrypted_buff, sizeof(decrypted_buff), key);
    if (decrypted_len == -1)
	{
	    std::cerr << "Decryption failed!" << std::endl;
	    return 1;
	}

    // 打印解密结果
    decrypted_buff[decrypted_len] = '\0'; // 添加字符串终止符
    std::cout << "Decrypted: " << decrypted_buff << std::endl;
    return 0;
}
*/