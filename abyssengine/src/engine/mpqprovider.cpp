#include "mpqprovider.h"
#include <libabyss/mpqstream.h>

AbyssEngine::MPQProvider::MPQProvider(std::filesystem::path &path) : _mpq(path) {

}

bool AbyssEngine::MPQProvider::Exists(std::filesystem::path &path) {
    return _mpq.HasFile(absolute(path).string());
}

LibAbyss::InputStream AbyssEngine::MPQProvider::Load(std::filesystem::path &path) {
    return _mpq.Load(path.string());
}
