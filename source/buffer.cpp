#include <nds.h>
#include <fat.h>
#include <functional>
#include <iostream>
#include <string>
#include <sys/dir.h>
#include <map>
#include "buffer.h"

using namespace std;

void Buffer::fill() {
	int dircount = -1;
	int buffercount = 0;
	//seleccion aleatoria de cancion
	int songcount = 0;
	map<int, int> songmap;
	//lectura recursiva de directorios
	function<bool(string)> parse = [&](string dir) -> bool {
		int pos;
		DIR *pdir;
		struct dirent *pent;
		bool isgroup = false;
		pdir = opendir(dir.c_str());
		if (pdir){
			while ((pent = readdir(pdir)) != NULL) {
				fileext = "";
	    		if ((strcmp(".", pent->d_name) == 0) || (strcmp("..", pent->d_name) == 0)) {
	        		continue;
	    		}
	    		if (pent->d_type == DT_DIR) {
	    			dircount++;
	    			isgroup = true;
	    			if (size != -1) {
	    				pos = dircountToBuffer(dircount);
	        			if (pos != -1) {
	        				if (items[pos].type == -1) {
	        					bufferitem group;
		        				group.type = 0;
		        				group.name = pent->d_name;
		        				group.path = dir + '/' + pent->d_name;
		        				items[pos] = group;
	        				}
	        				buffercount++;
	        			}
	        		}
	        		if (parse(dir + '/' + pent->d_name)) {
	        			return true;
	        		}
	    			if (buffercount > BUFFERSIZE) {
	    				return true;
	    			}
	    		}
	    		else if (!isgroup) {
	        		for (int i = 0; pent->d_name[i] != '\0'; i++) {
	        			fileext += pent->d_name[i];
	        			if (pent->d_name[i] == '.') {
	        				fileext = "";
	        			}
	        		}
	        		if (fileext == "sm") {
	        			pos = dircountToBuffer(dircount);
	        			if (size != -1 && items[pos].type == 0 && pos != -1) {
	        				bufferitem* song = &items[pos];
	        				song->type = 1;
	        				song->smpath = dir + '/' + pent->d_name;
	        				return false;
	        			} else if (random) {
	        				songmap.insert(pair<int, int>(songcount, dircount));
	        				songcount++;
	        				return false;
	        			}
	        		}
	    		}
			}
			closedir(pdir);
		}
		return false;
	};
	//encontrar total de elementos
	if (size == -1) {
		parse("/ddr");
		size = dircount + 1;
		dircount = -1;
	}
	//seleccionar cancion al azar
	if (random) {
		int r = rand() - 1;
		if (r < 0) {r = 0;}
		center = songmap.find(r / (RAND_MAX / (songcount)))->second;
		random = false;
	}
	//popular rueda
	parse("/ddr");
	//llenar espacios que faltan
	if (size < BUFFERSIZE) {
		for (int i = 0; i < BUFFERSIZE; i++) {
			if (items[i].type == -1) {
				int pos = bufferToFile(i);
				items[i] = items[dircountToBuffer(pos)];
			}
		}
	}
}

int Buffer::bufferToFile(int i) {
	int pos = center - (BUFFERSIZE / 2) + i;
	while (pos >= size) {
		pos = pos - size;
	}
	while (pos < 0) {
		pos = pos + size;
	}
	return pos;
}

int Buffer::dircountToBuffer(int i) {
	if (abs(center - i) <= (BUFFERSIZE / 2)) {
		return (i - center + (BUFFERSIZE / 2));
	}
	else if (abs(center + size - i) <= (BUFFERSIZE / 2)) {
		return (i - size - center + (BUFFERSIZE / 2));
	}
	else if (abs(center - size - i) <= (BUFFERSIZE / 2)) {
		return (i + size - center + (BUFFERSIZE / 2));
	}
	return -1;
}

void Buffer::setRandom() {
	random = true;
}