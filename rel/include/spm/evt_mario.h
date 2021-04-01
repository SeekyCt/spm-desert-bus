#pragma once

#include "evt_cmd.h"

namespace spm::evt_mario {

extern "C" {

EVT_DECLARE_USER_FUNC(evt_mario_key_on, 0)
EVT_DECLARE_USER_FUNC(evt_mario_key_off, 1)

}

}