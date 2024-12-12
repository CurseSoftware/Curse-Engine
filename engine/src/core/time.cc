#include "core/time.h"
#include "platform/platform.h"

namespace gravity {
namespace core {
namespace time {
void
clock::start() {
    start_time = platform::Platform::get()->get_absolute_time();
    elapsed_time = 0;
}

void
clock::update() {
    if (start_time != 0) {
        elapsed_time = platform::Platform::get()->get_absolute_time() - start_time;
    } 
}

void
clock::stop() {
    start_time = 0; 
}
}
}
}