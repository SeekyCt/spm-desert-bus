#pragma once

#include <types.h>

namespace spm::memory {

extern "C" {

void *__memAlloc(u32 heap, u32 size);
void __memFree(u32 heap, void *ptr);

}

}
