#pragma once

#include <vector>
#include <esp32-hal.h>
#include <Stream.h>

#include <bytelang/Device.hpp>
#include <bytelang/Result.hpp>
#include <bytelang/Reader.hpp>


namespace bytelang {
    typedef Result (*Instruction)(Reader &, Device &);

    class StreamInterpreter {

    private:
        volatile bool has_aborted = false;
        volatile bool has_paused = false;

        const std::vector<Instruction> instructions_table;

    public:

        explicit StreamInterpreter(std::vector<Instruction> &&instructionsTable) : instructions_table(instructionsTable) {
        }

        static StreamInterpreter &getInstance();

        void abort();

        void setPaused(bool new_state);

        Result run(Stream &stream, Device &device);
    };

}
