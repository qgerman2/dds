#include <string>
#include <vector>

typedef std::vector<u16 *> measure;
void parseSong(std::string path);
std::vector<measure> parseNotes(std::string data);

struct t_pair {
	std::string key;
	std::string value;
};

static const u16 normal[4] {
	0b0001000000000000,
	0b0000000100000000,
	0b0000000000010000,
	0b0000000000000001,
};
static const u16 holdhead[4] {
	0b0010000000000000,
	0b0000001000000000,
	0b0000000000100000,
	0b0000000000000010,
};
static const u16 holdtail[4] {
	0b0011000000000000,
	0b0000001100000000,
	0b0000000000110000,
	0b0000000000000011,
};
static const u16 rollhead[4] {
	0b0100000000000000,
	0b0000010000000000,
	0b0000000001000000,
	0b0000000000000100,
};
static const u16 mine[4] {
	0b0101000000000000,
	0b0000010100000000,
	0b0000000001010000,
	0b0000000000000101,
};