#ifndef node_h
#define node_h

#include <iostream>

class node {
	int id;
	uint64_t sendTimestamp;
	int giftType;

	node() 
	:id(0),
	sendTimestamp(0),
	giftType(0){
	}
	node(int id_, uint64_t sendTimestamp_, int giftType_)
		:id(id_),
		sendTimestamp(sendTimestamp_),
		giftType(giftType_)
	{

	}
};

#endif