#pragma once

#include <Stream.h>


namespace bytelang {
    class Reader {


    public:
        Stream &stream;

        explicit Reader(Stream &stream) : stream(stream) {
        }

        template<typename T> inline void read(T &buffer) {
            stream.readBytes((uint8_t *) &buffer, sizeof(T));
        }

    };
}