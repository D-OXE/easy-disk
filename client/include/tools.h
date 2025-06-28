#pragma once
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief 一个工具集合,在应用层实现对即将发送到网络中的数据进行加密,分两种,长加密,短加密.
 * @details 每都会用到密钥,后续需要注意密钥分发的问题(TODO)
 *
 * @author youth (doxe20191817@gmail.com)
 * @date 2025-06-28 12:11:12
 */

// 短消息加密
std::string encode_string(const std::string& plaintext, const std::string& key);

// 短消息传输解密
std::string decode_string(const std::string& ciphertext, const std::string& key);

// 文件传输的加密
int encode_data(const uint8_t* input,  // 输入数据（明文）
		size_t input_len,      // 输入数据长度
		uint8_t* output,       // 输出缓冲区（存放密文）
		size_t output_max_len, // 输出缓冲区最大容量
		const std::string& key // 16字节密钥
);

// 文件传输的解密函数
int decode_data(const uint8_t* input,  // 输入数据（密文，含IV）
		size_t input_len,      // 输入数据长度
		uint8_t* output,       // 输出缓冲区（存放明文）
		size_t output_max_len, // 输出缓冲区最大容量
		const std::string& key // 16字节密钥
);