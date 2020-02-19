#include <string>
#include <vector>
#include <variant>

struct t_pair {
	std::string key;
	std::string value;
};
typedef std::vector<u16 *> measure;
typedef std::vector<measure> notedata;
struct t_bpm {
	u32 beatf = 0;
	u32 bpmf = 0;
};
typedef std::vector<struct t_bpm> bpmdata;
typedef std::vector<struct t_pair> metadata;
class songdata {
	public:
		notedata notes;
		metadata* tags;
		bpmdata bpms;
		bpmdata stops;
};
notedata parseNotes(std::string* data);
bpmdata parseBPMS(std::string* data, bool isStops);
metadata parseSimFile(std::string path, bool partial);
songdata parseSong(metadata* tags);
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
	0b0100000000000000,
	0b0000010000000000,
	0b0000000001000000,
	0b0000000000000100,
};
static const u16 rollhead[4] {
	0b0011000000000000,
	0b0000001100000000,
	0b0000000000110000,
	0b0000000000000011,
};
static const u16 mine[4] {
	0b1000000000000000,
	0b0000100000000000,
	0b0000000010000000,
	0b0000000000001000,
};