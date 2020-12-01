#include "gdflatbuffers.hpp"

namespace godot {

FlatBuffers::FlatBuffers() {
	builder = flatbuffers::FlatBufferBuilder(1024);
	file = File::_new();
	dir = Directory::_new();
}

FlatBuffers::~FlatBuffers() {
	builder.Clear();
	file->free();
	dir->free();
}

FlatBuffers &FlatBuffers::instance() {
	static FlatBuffers _instance;
	return _instance;
}

void FlatBuffers::Save(const String &path, Bufferable *obj) {
	instance().mtx.lock();
	instance().builder.Clear();
	obj->mkbuffer(instance().builder);
	PoolByteArray _out;
	auto buf = instance().builder.GetBufferPointer();
	for (int i = 0; i < instance().builder.GetSize(); i++)
		_out.append(buf[i]);
	if (instance().file->file_exists(path))
		instance().dir->remove(path);
	instance().file->open(path, File::WRITE);
	instance().file->store_buffer(_out);
	instance().file->close();
	instance().mtx.unlock();
}

const uint8_t *FlatBuffers::Load(const String &path) {
	instance().mtx.lock();
	const uint8_t *data;
	if (instance().file->file_exists(path)) {
		instance().file->open(path, File::READ);
		PoolByteArray _raw = instance().file->get_buffer(instance().file->get_len());
		data = _raw.read().ptr();
		instance().file->close();
	}
	instance().mtx.unlock();
	return data;
}

} // namespace godot
