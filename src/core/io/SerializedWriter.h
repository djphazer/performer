#pragma once

#include <cstdint>
#include <functional>

class SerializedWriter {
public:
    using Writer = std::function<void(const void *, size_t)>;

    SerializedWriter(Writer writer) :
        _writer(writer)
    {}

    template<typename T>
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    void write(const void *data, size_t len) {
        _writer(data, len);
    }

private:
    Writer _writer;
};
