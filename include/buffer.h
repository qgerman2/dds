#ifndef BUFFER
#define BUFFER
#define BUFFERSIZE 19
#include <string>
typedef struct bufferitem{
	int type = -1;
	std::string name;
	std::string path;
	std::string smpath;
} bufferitem;
class Buffer {
private:
	bool random = false;
public:
	int cursor = BUFFERSIZE / 2;
	bufferitem items[BUFFERSIZE];
	int center = 0;
	std::string fileext;
	int size = -1;
	void fill();
	int bufferToFile(int i);
	int dircountToBuffer(int i);
	void setRandom();
};
#endif