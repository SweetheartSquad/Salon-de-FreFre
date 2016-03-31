#pragma once

#include <Tracks.h>
#include <FileUtils.h>
#include <assert.h>

Track::Track(Json::Value _json) :
	audioTrack(_json.get("audio", "DEFAULT").asString()),
	needsInput(_json.get("needsInput", false).asBool()),
	data(_json.get("data", "NO_DATA").asString())
{
}

Tracks::Tracks(){
	Json::Reader reader;
	Json::Value json;
	bool parsingSuccessful;
	
	parsingSuccessful = reader.parse(sweet::FileUtils::readFile("assets/tracks.json"), json);

	assert(parsingSuccessful);

	for(auto j : json["tracks"]){
		tracks.push_back(Track(j));
	}
}