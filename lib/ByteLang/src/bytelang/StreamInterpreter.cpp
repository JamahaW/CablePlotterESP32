#include <bytelang/StreamInterpreter.hpp>
#include <bytelang/Instructions.hpp>


bytelang::StreamInterpreter &bytelang::StreamInterpreter::getInstance() {
    static StreamInterpreter interpreter(
            {
                    instructions::quit,
                    instructions::delay_ms,
                    instructions::set_speed,
                    instructions::set_speed_multiplication,
                    instructions::set_progress,
                    instructions::set_position,
                    instructions::set_active_tool
            }
    );
    return interpreter;
}

bytelang::Result bytelang::StreamInterpreter::run(Stream &stream, bytelang::Device &device) {
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

void bytelang::StreamInterpreter::setPaused(bool new_state) {
    has_paused = new_state;
}

void bytelang::StreamInterpreter::abort() {
    has_aborted = true;
}
