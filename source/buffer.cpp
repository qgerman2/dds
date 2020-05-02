#include <nds.h>
#include <fat.h>
#include <functional>
#include <iostream>
#include <string>
#include <sys/dir.h>
#include <map>
#include "globals.h"
#include "buffer.h"
#include "play_score.h"

using namespace std;

void Buffer::fill(string focus) {
	int dircount = -1;
	int buffercount = 0;
	int focuscount = -1;
	//seleccion aleatoria de cancion
	int songcount = 0;
	map<int, int> songmap;
	//lectura recursiva de directorios
	function<bool(string, int)> parse = [&](string dir, int depth) -> bool {
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
	    			if (depth == 0) {
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
		        		} else {
		        			if (focus == pent->d_name) {
	        					focuscount = dircount;
	        				}
		        		}
		        		if (parse(dir + '/' + pent->d_name, depth + 1)) {
		        			return true;
		        		}
		    			if (buffercount > BUFFERSIZE) {
		    				return true;
		    			}
		    		} else {
		    			if (size != -1) {
		    				pos = dircountToBuffer(dircount);
		    				bufferitem* group = &items[pos];
							group->type = 2;
		    			}
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
							parseSimFile(&song->song, song->smpath);
							ScoreLoad(song->path, &song->scores, &song->song);
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
		parse(bufferpath.c_str(), 0);
		size = dircount + 1;
		dircount = -1;
		if (size == 0) {
			//no hay elementos
			size = 1;
			bufferitem empty;
			empty.type = 0;
			empty.name = "No songs found";
			items[BUFFERSIZE / 2] = empty;
		}
	}
	//seleccionar cancion al azar
	if (random) {
		int r = rand() - 1;
		if (r < 0) {r = 0;}
		center = songmap.find(r / (RAND_MAX / songcount))->second;
		random = false;
	} else {
		center = buffer_center;
		cursor = buffer_cursor;
		if (focuscount != -1) {
			center = focuscount;
		}
	}
	//popular buffer
	parse(bufferpath.c_str(), 0);
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

void Buffer::clear() {
	size = -1;
	buffer_center = 0;
	buffer_cursor = BUFFERSIZE / 2;
	for (int i = 0; i < BUFFERSIZE; i++) {
		items[i].type = -1;
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
