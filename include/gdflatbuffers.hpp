#ifndef __GD_FLATBUFFERS__
#define __GD_FLATBUFFERS__

#include <iostream>
#include <mutex>

#include <Directory.hpp>
#include <File.hpp>
#include <Godot.hpp>

#include "flatbuffers/flatbuffers.h"

namespace godot {

class Bufferable {
public:
	virtual void mkbuffer(flatbuffers::FlatBufferBuilder &builder){};
};

class FlatBuffers {
private:
	FlatBuffers();
	~FlatBuffers();
	static FlatBuffers &instance();

	flatbuffers::FlatBufferBuilder builder;
	File *file;
	Directory *dir;

	// mutex to only allow single access to save/load
	std::mutex mtx;

public:
	static void Save(const String &path, Bufferable *obj);
	const static uint8_t *Load(const String &path);
};
} // namespace godot

#endif