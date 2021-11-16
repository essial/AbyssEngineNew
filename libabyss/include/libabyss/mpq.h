#ifndef ABYSS_MPQ_H
#define ABYSS_MPQ_H

#include <fstream>
#include <vector>
#include <string>

namespace LibAbyss {

    namespace BlockFlag {
        enum BlockFlag : uint32_t {
            Imploded = 0x00000100,
            Compressed = 0x00000200,
            Encrypted = 0x00010000,
            FixKey = 0x00020000,
            PatchFile = 0x00100000,
            SingleUnit = 0x01000000,
            Deleted = 0x02000000,
            SectorCRC = 0x04000000,
            Exists = 0x80000000
        };
    }

    class MPQ {
    public:

        struct MPQHashEntry {
            uint32_t HashA;
            uint32_t HashB;
            uint16_t Locale;
            uint16_t Platform;
            uint32_t BlockIndex;
        };

        struct MPQHeader {
            uint8_t Magic[4];
            uint32_t HeaderSize;
            uint32_t ArchiveSize;
            uint16_t FormatVersion;
            uint16_t BlockSize;
            uint32_t HashTableOffset;
            uint32_t BlockTableOffset;
            uint32_t HashTableEntries;
            uint32_t BlockTableEntries;
        };

        struct MPQBlock {
            uint32_t FilePosition;
            uint32_t FileSizeCompressed;
            uint32_t FileSizeUncompressed;
            BlockFlag::BlockFlag Flags;
        };


        /// Creates an MPQ based on the supplied source stream.
        /// \param source The stream source that contains the raw MPQ data.
        explicit MPQ(std::ifstream mpqPath);

        /// Proxy constructor that creates an MPQ based on the specified filename.
        /// \param mpqPath Path to the MPQ file to load.
        explicit MPQ(const std::filesystem::path& mpqPath) : MPQ(
                std::ifstream(mpqPath, std::fstream::in | std::fstream::binary)) {};

    private:
        MPQHeader _header;
        std::vector<MPQHashEntry> _hashEntries;
        std::vector<MPQBlock> _blockEntries;
    };

} // namespace LibAbyss

#endif // ABYSS_MPQ_H
