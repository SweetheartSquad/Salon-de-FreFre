#pragma once

#include <json\json.h>
#include <node/Node.h>


class Track : public Node{
public:
	std::string audioTrack;
	bool needsInput;
	std::string data;
	
	Track(Json::Value _json);


};

class Tracks : public Node{
public:
	Tracks();

	std::vector<Track> tracks;
};