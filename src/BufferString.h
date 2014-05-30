#ifndef ECAP_ADAPTER_BUFFERSTRING_H
#define ECAP_ADAPTER_BUFFERSTRING_H

class BufferString {
	
	public:
		char* start;
		char* pos;
		char* last;
		
		BufferString(){}
		BufferString(char *point):start(point), pos(point), last(point){}
		
		BufferString(char *point, int size):start(point), pos(point), last(point+size){}
		
};

#endif