#include "../include/libabyss/mpq.h"
#include "../include/libabyss/crypto.h"

#include <spdlog/spdlog.h>

LibAbyss::MPQ::MPQ(std::ifstream source) : _header(), _blockEntries(), _hashEntries() {
    source.seekg(0, std::ios_base::beg);

    // Read in the MPQ header
    source.read((char *) &_header, sizeof(MPQHeader));

    // Verify the magic header
    if (std::string_view((char *) &_header.Magic[0], 4) != "MPQ\x1A") {
        throw std::runtime_error("Invalid MPQ header");
    }

    // Load the hash table
    _hashEntries.reserve(_header.HashTableEntries);
    source.seekg(_header.HashTableOffset, std::ios_base::beg);
    auto hashData = LibAbyss::Crypto::DecryptTableFromFile(source, _header.HashTableEntries, "(hash table)");
    for (auto i = 0, n = 0; i < _header.HashTableEntries; i++, n += 4) {
        _hashEntries.push_back({hashData[n], hashData[n + 1], (uint16_t) (hashData[n + 2] >> 16),
                                (uint16_t) (hashData[n + 2] & 0xFFFF), hashData[n + 3],});
    }

    SPDLOG_INFO("x {0}", _hashEntries.size());
    // Load the block table
    _blockEntries.reserve(_header.BlockTableEntries);
    source.seekg(_header.BlockTableOffset, std::ios_base::beg);
    auto blockData = LibAbyss::Crypto::DecryptTableFromFile(source, _header.HashTableEntries, "(block table)");
    for (auto i = 0, n = 0; i < _header.BlockTableEntries; i++, n += 4) {
        _blockEntries.push_back({(uint32_t) blockData[n], (uint32_t) blockData[n + 1], (uint32_t) blockData[n + 2],
                                 (BlockFlag) blockData[n + 3]});
    }
}
