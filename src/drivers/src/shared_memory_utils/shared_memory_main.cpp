/**
 * @file shared_memory_main.cpp
 * Definitions for owner of shared memory
 */

#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "constants/simulator.h"

namespace drivers {

using namespace boost::interprocess;

SharedMemoryMain::SharedMemoryMain(const std::string &shared_memory_name,
                                   bool is_player_running,
                                   int64_t instruction_count,
                                   const transfer_state::State &transfer_state)
    : shared_memory_name(shared_memory_name),
      // Creating shared memory
      shared_memory(create_only, shared_memory_name.c_str(),
                    Constants::Simulator::SHARED_BUFFER_SIZE) {
    // Constructing unique instance of SharedBuffer in shared memory
    this->shared_memory.construct<SharedBuffer>(unique_instance)(
        is_player_running, instruction_count, instruction_count,
        transfer_state);
}

SharedBuffer *SharedMemoryMain::getBuffer() {
    return this->shared_memory.find<SharedBuffer>(unique_instance).first;
}

SharedMemoryMain::~SharedMemoryMain() {
    this->shared_memory.destroy<SharedBuffer>(unique_instance);
    shared_memory_object::remove(shared_memory_name.c_str());
}
} // namespace drivers
