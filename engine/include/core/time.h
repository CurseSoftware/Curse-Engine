#pragma once
#include "core/defines.h"

namespace gravity {
namespace  core {
namespace time {

struct clock {
    double start_time;
    float elapsed_time;

    void update();
    void start();
    void stop();
};

} // time namespace
} // core namespace
} // gravity namespace