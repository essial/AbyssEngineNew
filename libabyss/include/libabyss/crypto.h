#ifndef ABYSS_CRYPTO_H
#define ABYSS_CRYPTO_H

#include <cstdint>
#include <span>
#include <fstream>

namespace LibAbyss {

    class Crypto {
    public:
        static void Decrypt(std::span<uint8_t> data, uint32_t seed);

        static void DecryptBytes(std::span<uint8_t> data, uint32_t seed);

        static uint32_t HashString(std::string_view key, uint32_t hashType);

        static std::vector<uint32_t> DecryptTableFromFile(std::ifstream &source, uint32_t size, std::string_view name);

        static void Initialize();

    private:
        static uint64_t HashFileName(const std::string &key);

        static uint32_t _cryptoBuffer[0x500];
    };

}

#endif //ABYSS_CRYPTO_H
