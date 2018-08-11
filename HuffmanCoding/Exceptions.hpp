#include <stdexcept>

struct DecodingFailed : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct FileFormatError : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct UnCompressFailed : std::runtime_error {
	UnCompressFailed(const std::string &s, std::size_t rem) : std::runtime_error(s), remSize(rem) { }
	std::size_t remSize;
};
