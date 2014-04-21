#include <string>

class BufferString {
	
	public:
		char* start;
		char* pos;
		char* last;
		
		BufferString(){}
		BufferString(char *point):start(point), pos(point), last(point){}
		
		BufferString(char *point, int size):start(point), pos(point), last(point+size){}
		
};