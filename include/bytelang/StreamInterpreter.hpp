#pragma once

#include <array>
#include <Stream.h>

#include "Result.hpp"
#include "Reader.hpp"

namespace bytelang {
    typedef Result (*Instruction)(Reader &);

    template<int IC>
    class StreamInterpreter {

    private:
        volatile bool has_aborted = false;
        volatile bool has_paused = false;

        const std::array<Instruction, IC> instructions_table;

    public:

        explicit StreamInterpreter(
                std::array<Instruction, IC> instructionsTable)
                : instructions_table(instructionsTable) {}

        void abort() {
            has_aborted = true;
        }

        void setPaused(bool new_state) {
            has_paused = new_state;
        }

        Result run(Stream &stream) {
            uint8_t instruction_code;
            Result result;
            Reader reader(stream);

            if (stream.read() != 0x01) {
                return Result::ERROR_INVALID_OFFSET_VALUE;
            }

            has_aborted = false;

            while (true) {
                if (not has_paused) {
                    instruction_code = stream.read();

                    if (instruction_code > instructions_table.size()) {
                        return Result::ERROR_INVALID_INSTRUCTION_CODE;
                    }

                    result = instructions_table[instruction_code](reader);
                }

                if (result != Result::OK) {
                    return result;
                }

                if (has_aborted) {
                    return Result::ABORT;
                }

                delay(1);
            }
        }
    };

}
