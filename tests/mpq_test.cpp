#include <gmock/gmock.h>
#include <sstream>
#include <libabyss/mpq.h>
#include <libabyss/mpqstream.h>
#include <stdexcept>

using testing::Eq;
using testing::ThrowsMessage;

class MpqTest : public testing::Test {
  public:
    MpqTest() {
        _fname = std::tmpnam(nullptr);
        _fname += ".mpq";
    }

    ~MpqTest() override { std::filesystem::remove(_fname); }

  protected:
    void WriteMPQ(std::function<void(HANDLE mpq)> data) {
        HANDLE mpq;
        ASSERT_TRUE(SFileCreateArchive(_fname.c_str(), MPQ_CREATE_LISTFILE, 100, &mpq));
        data(mpq);
        SFileCloseArchive(mpq);
    }
    static void WriteIn(HANDLE mpq, const std::string &fname, std::string_view data) {
        HANDLE f;
        ASSERT_TRUE(SFileCreateFile(mpq, fname.c_str(), 0, data.size(), 0, 0, &f));
        ASSERT_TRUE(SFileWriteFile(f, data.data(), data.length(), 0));
        ASSERT_TRUE(SFileFinishFile(f));
    }

    std::string _fname;
};

TEST_F(MpqTest, NonExistingMPQ) {
    EXPECT_THAT([&] { LibAbyss::MPQ mpq(_fname); }, ThrowsMessage<std::runtime_error>(Eq("Error occurred while opening MPQ " + _fname)));
}

TEST_F(MpqTest, NonExistingFile) {
    WriteMPQ([](HANDLE mpq) {});
    LibAbyss::MPQ mpq(_fname);
    EXPECT_THAT([&] { mpq.Load("non-existing.txt"); }, ThrowsMessage<std::runtime_error>(Eq("Failed to open file 'non-existing.txt' from MPQ")));
}

TEST_F(MpqTest, SmallFile) {
    WriteMPQ([](HANDLE mpq) { WriteIn(mpq, "small.txt", "abcd"); });

    LibAbyss::MPQ mpq(_fname);

    LibAbyss::InputStream small = mpq.Load("small.txt");
    EXPECT_EQ(small.tellg(), 0);
    EXPECT_EQ(small.get(), 'a');
    EXPECT_EQ(small.tellg(), 1);
    EXPECT_EQ(small.get(), 'b');
    small.seekg(-1, std::ios_base::cur);
    EXPECT_EQ(small.get(), 'b');
    small.seekg(3);
    EXPECT_EQ(small.get(), 'd');
    EXPECT_FALSE(small.eof());
    EXPECT_EQ(small.get(), -1);
    EXPECT_TRUE(small.eof());
    small.clear();
    small.seekg(-2, std::ios_base::cur);
    EXPECT_EQ(small.get(), 'c');
    small.seekg(-2, std::ios_base::end);
    EXPECT_EQ(small.get(), 'c');
    EXPECT_EQ(small.tellg(), 3);
}

TEST_F(MpqTest, BigFile) {
    std::stringstream s;
    for (int i = 0; i < 10000; ++i) {
        s << i << ' ';
    }
    std::string source_str = s.str();
    std::string_view source = source_str;
    EXPECT_EQ(source.length(), 48890);
    EXPECT_EQ(source.substr(40000, 20), "8222 8223 8224 8225 ");
    // Near end of the chunk (+2048)
    EXPECT_EQ(source.substr(42040, 20), "8630 8631 8632 8633 ");

    WriteMPQ([&](HANDLE mpq) { WriteIn(mpq, "big.txt", source); });

    LibAbyss::MPQ mpq(_fname);
    LibAbyss::InputStream big = mpq.Load("big.txt");
    LibAbyss::MPQStream* strbuf = dynamic_cast<LibAbyss::MPQStream*>(big.rdbuf());
    ASSERT_NE(strbuf, nullptr);
    big.seekg(40000);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    int x;
    big >> x;
    big >> x;
    big >> x;
    EXPECT_EQ(x, 8224);
    big.seekg(42040);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    big.seekg(-10, std::ios_base::cur);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 40000);
    big >> x;
    EXPECT_EQ(x, 8628);
    big >> x;
    big >> x;
    big >> x;
    EXPECT_EQ(x, 8631);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 42048);
    big.seekg(-10, std::ios_base::cur);
    EXPECT_EQ(strbuf->StartOfBlockForTesting(), 42039);
    big >> x;
    EXPECT_EQ(x, 8630);

    // Finally, read the whole file
    big.seekg(0);
    for (int i = 0; i < 10000; ++i) {
        big >> x;
        EXPECT_FALSE(big.eof());
        EXPECT_EQ(i, x);
    }
    big >> x;
    EXPECT_TRUE(big.eof());
}
