#pragma once

#include "evt_cmd.h"

namespace spm::evt_msg {

extern "C" {

EVT_DECLARE_USER_FUNC(evt_msg_print, 4)
EVT_DECLARE_USER_FUNC(evt_msg_continue, 0)

}

}