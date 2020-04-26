#include <nds.h>
#include <iostream>
#include <string>
#include "cache.h"
#include "artwork.h"

using namespace std;

bool loadCache(string file, u16* dest, uint width, uint height) {
	string name = file.substr(0, file.find_last_of("."));
	if (!loadArtwork(name + ".dds", dest, width, height)) {
		if (loadArtwork(file, dest, width, height)) {
			exportArtwork(name + ".dds", dest, width, height);
			return true;
		} else {
			return false;
		}
	}
	return true;
}