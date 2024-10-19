#pragma once

#include <vector>
#include <esp32-hal.h>
#include <Stream.h>

#include <bytelang/Result.hpp>
#include <bytelang/Reader.hpp>

// TODO решить это
namespace cableplotter {
    class Device;
}

namespace bytelang {
    typedef Result (*Instruction)(Reader &, cableplotter::Device &);

    class StreamInterpreter {

    private:
        volatile bool has_aborted = false;
        volatile bool has_paused = false;

        const std::vector<Instruction> instructions_table;

    public:

        explicit StreamInterpreter(std::vector<Instruction> &&instructionsTable) : instructions_table(instructionsTable) {
        }

        void abort() {
            has_aborted = true;
        }

        void setPaused(bool new_state) {
            has_paused = new_state;
        }

        Result run(Stream &stream, cableplotter::Device &device) {
            uint8_t instruction_code;
            Result result;
            Reader reader(stream);

            if (stream.read() != 0x01) {
                return Result::FATAL_ERROR_INVALID_OFFSET_VALUE;
            }

            has_aborted = false;
            has_paused = false;

            while (true) {
                delay(1);

                if (has_aborted) {
                    return Result::FATAL_ABORT;
                }

                if (has_paused) {
                    continue;
                }

                instruction_code = stream.read();

                if (instruction_code > instructions_table.size()) {
                    return Result::FATAL_ERROR_INVALID_INSTRUCTION_CODE;
                }

                result = instructions_table[instruction_code](reader, device);

                if (result != Result::OK) {
                    return result;
                }
            }
        }
    };

}
