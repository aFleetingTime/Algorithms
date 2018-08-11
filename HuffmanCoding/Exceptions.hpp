#include <stdexcept>

struct DecodingFailed : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct FileFormatError : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct FileDataLost : std::runtime_error {
	FileDataLost(const std::string &s, std::size_t e, std::size_t sz) : std::runtime_error(s), expect(e), size(sz) { }
	std::size_t expect, size;
};

struct UnCompressFailed : std::runtime_error {
	UnCompressFailed(const std::string &s, std::size_t rem) : std::runtime_error(s), remSize(rem) { }
	std::size_t remSize;
};
