#ifndef BUFFER
#define BUFFER
#define BUFFERSIZE 19
#include <string>
#include <vector>
#include "parse.h"
#include "play_score.h"
typedef struct bufferitem{
	int type = -1;
	std::string name;
	std::string path;
	std::string smpath;
	songdata song;
	std::vector<score_p> scores;
} bufferitem;
class Buffer {
private:
	bool random = false;
public:
	bufferitem items[BUFFERSIZE];
	std::string fileext;
	int center;
	int cursor = BUFFERSIZE / 2;
	int size = -1;
	void fill();
	int bufferToFile(int i);
	int dircountToBuffer(int i);
	void setRandom();
	void clear();
};
#endif
