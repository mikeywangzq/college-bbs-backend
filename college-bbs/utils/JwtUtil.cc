#include "JwtUtil.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>

// JWT密钥 - 实际生产环境应该从环境变量或配置文件读取
const std::string JwtUtil::SECRET_KEY = "college_bbs_secret_key_2024";

// Token有效期：7天 = 7 * 24 * 60 * 60秒
const int JwtUtil::EXPIRATION_TIME = 7 * 24 * 60 * 60;

// Base64字符表
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// Base64编码
static std::string base64Encode(const unsigned char* input, size_t length) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (length--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

// Base64解码
static std::string base64Decode(const std::string& encoded_string) {
    size_t in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') &&
           (isalnum(encoded_string[in_]) || (encoded_string[in_] == '+') || (encoded_string[in_] == '/'))) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                size_t pos = base64_chars.find(char_array_4[i]);
                if (pos == std::string::npos) {
                    return ""; // Invalid base64 character
                }
                char_array_4[i] = pos;
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++) {
            size_t pos = base64_chars.find(char_array_4[j]);
            if (pos == std::string::npos) {
                pos = 0; // Treat as padding
            }
            char_array_4[j] = pos;
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

std::string JwtUtil::base64UrlEncode(const std::string& input) {
    std::string b64 = base64Encode(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());

    // URL安全的Base64编码：替换 + 为 -, / 为 _, 移除 =
    for (auto& c : b64) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }

    // 移除末尾的 =
    b64.erase(std::remove(b64.begin(), b64.end(), '='), b64.end());

    return b64;
}

std::string JwtUtil::base64UrlDecode(const std::string& input) {
    std::string b64 = input;

    // 还原标准Base64编码
    for (auto& c : b64) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }

    // 补齐 =
    while (b64.length() % 4 != 0) {
        b64 += '=';
    }

    return base64Decode(b64);
}

std::string JwtUtil::hmacSha256(const std::string& key, const std::string& data) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    HMAC(EVP_sha256(),
         key.c_str(), key.length(),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.length(),
         digest, &digest_len);

    return std::string(reinterpret_cast<char*>(digest), digest_len);
}

std::string JwtUtil::generateToken(int user_id, const std::string& username) {
    // 1. 创建Header
    Json::Value header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";

    Json::StreamWriterBuilder headerBuilder;
    headerBuilder["indentation"] = "";
    std::string headerStr = Json::writeString(headerBuilder, header);
    std::string headerEncoded = base64UrlEncode(headerStr);

    // 2. 创建Payload
    Json::Value payload;
    payload["user_id"] = user_id;
    payload["username"] = username;

    // 设置过期时间
    auto now = std::chrono::system_clock::now();
    auto exp = std::chrono::system_clock::to_time_t(now) + EXPIRATION_TIME;
    payload["exp"] = static_cast<Json::Int64>(exp);

    // 签发时间
    auto iat = std::chrono::system_clock::to_time_t(now);
    payload["iat"] = static_cast<Json::Int64>(iat);

    Json::StreamWriterBuilder payloadBuilder;
    payloadBuilder["indentation"] = "";
    std::string payloadStr = Json::writeString(payloadBuilder, payload);
    std::string payloadEncoded = base64UrlEncode(payloadStr);

    // 3. 创建签名
    std::string data = headerEncoded + "." + payloadEncoded;
    std::string signature = hmacSha256(SECRET_KEY, data);
    std::string signatureEncoded = base64UrlEncode(signature);

    // 4. 组合JWT
    return headerEncoded + "." + payloadEncoded + "." + signatureEncoded;
}

bool JwtUtil::verifyToken(const std::string& token, int& user_id, std::string& username) {
    try {
        // 1. 分割Token
        size_t firstDot = token.find('.');
        size_t secondDot = token.find('.', firstDot + 1);

        if (firstDot == std::string::npos || secondDot == std::string::npos) {
            return false;
        }

        std::string headerEncoded = token.substr(0, firstDot);
        std::string payloadEncoded = token.substr(firstDot + 1, secondDot - firstDot - 1);
        std::string signatureEncoded = token.substr(secondDot + 1);

        // 2. 验证签名
        std::string data = headerEncoded + "." + payloadEncoded;
        std::string expectedSignature = hmacSha256(SECRET_KEY, data);
        std::string expectedSignatureEncoded = base64UrlEncode(expectedSignature);

        if (signatureEncoded != expectedSignatureEncoded) {
            return false;
        }

        // 3. 解析Payload
        std::string payloadStr = base64UrlDecode(payloadEncoded);

        Json::CharReaderBuilder readerBuilder;
        Json::Value payload;
        std::string errs;
        std::istringstream s(payloadStr);

        if (!Json::parseFromStream(readerBuilder, s, &payload, &errs)) {
            return false;
        }

        // 4. 检查过期时间
        if (payload.isMember("exp")) {
            Json::Int64 exp = payload["exp"].asInt64();
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            if (now > exp) {
                return false; // Token已过期
            }
        }

        // 5. 提取用户信息
        if (payload.isMember("user_id") && payload.isMember("username")) {
            user_id = payload["user_id"].asInt();
            username = payload["username"].asString();
            return true;
        }

        return false;
    } catch (...) {
        return false;
    }
}
