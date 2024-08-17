#pragma once

#include <array>
#include <Stream.h>

#include "Result.hpp"
#include "Reader.hpp"

namespace bytelang {
    typedef Result (*Instruction)(Reader &);

    template<int IC>
    class StreamInterpreter {
    public:

        const std::array<Instruction, IC> instructions_table;

        explicit StreamInterpreter(
                std::array<Instruction, IC> instructionsTable)
                : instructions_table(instructionsTable) {}

        Result run(Stream &stream) {
            uint8_t instruction_code;
            Result result;
            Reader reader(stream);

            if (stream.read() != 0x01) {
                return Result::ERROR_INVALID_OFFSET_VALUE;
            }

            while (true) {
                instruction_code = stream.read();

                if (instruction_code > instructions_table.size()) {
                    return Result::ERROR_INVALID_INSTRUCTION_CODE;
                }

                result = instructions_table[instruction_code](reader);

                if (result != Result::OK) {
                    return result;
                }
            }
        }
    };

}
