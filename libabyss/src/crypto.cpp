#include <vector>
#include "libabyss/crypto.h"

uint32_t LibAbyss::Crypto::_cryptoBuffer[0x500] = {};

[[maybe_unused]] static int cryptoInit = (LibAbyss::Crypto::Initialize(), 0); // NOLINT(cert-err58-cpp)

void LibAbyss::Crypto::Initialize() {
    uint32_t seed = 0x00100001;

    for (int index1 = 0; index1 < 0x100; index1++) {
        int index2 = index1;
        for (int i = 0; i < 5; i++) {
            seed = (seed * 125 + 3) % 0x2AAAAB;
            const uint32_t temp1 = (seed & 0xFFFF) << 0x10;

            seed = (seed * 125 + 3) % 0x2AAAAB;
            const uint32_t temp2 = seed & 0xFFFF;

            _cryptoBuffer[index2] = temp1 | temp2;

            index2 += 0x100;
        }
    }
}

void LibAbyss::Crypto::Decrypt(std::span<uint8_t> data, uint32_t seed) {
    uint32_t seed2 = 0xEEEEEEEE;

    for (unsigned char &i: data) {
        seed2 += _cryptoBuffer[0x400 + (seed & 0xFF)];
        uint32_t result = i;
        result ^= seed + seed2;

        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;
        i = result;
    }
}

void LibAbyss::Crypto::DecryptBytes(std::span<uint8_t> data, uint32_t seed) {
    uint32_t seed2 = 0xEEEEEEEE;

    for (int i = 0; i < data.size() - 3; i += 4) {
        seed2 += _cryptoBuffer[0x400 + (seed & 0xFF)];
        uint32_t result = *((uint32_t *) &data[i]);
        result ^= seed + seed2;
        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;

        data[i + 0] = (uint8_t) (result & 0xff);
        data[i + 1] = (uint8_t) ((result >> 8) & 0xff);
        data[i + 2] = (uint8_t) ((result >> 16) & 0xff);
        data[i + 3] = (uint8_t) ((result >> 24) & 0xff);
    }
}

uint32_t LibAbyss::Crypto::HashString(std::string_view key, uint32_t hashType) {
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;

    /* prepare seeds. */
    for (char i: key) {
        const char ch = (char) toupper(i);
        seed1 = _cryptoBuffer[(hashType * 0x100) + ((uint32_t) ch)] ^ (seed1 + seed2);
        seed2 = ((uint32_t) ch) + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}

uint64_t LibAbyss::Crypto::HashFileName(const std::string &key) {
    const uint32_t a = HashString(key, 1);
    const uint32_t b = HashString(key, 2);

    return (((uint64_t) a) << 32) | ((uint64_t) b);
}

std::vector<uint32_t> LibAbyss::Crypto::DecryptTableFromFile(std::ifstream &source, uint32_t size, std::string_view name) {
    uint32_t seed = HashString(name, 3);
    uint32_t seed2 = 0xEEEEEEEE;
    size *= 4;

    std::vector<uint32_t> table(size);

    for (int i = 0; i < size; i++) {
        seed2 += _cryptoBuffer[0x400 + (seed & 0xff)];
        uint32_t result;
        source.read((char *) &result, sizeof(uint32_t));
        result ^= seed + seed2;
        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;
        table[i] = result;
    }

    return table;
}
