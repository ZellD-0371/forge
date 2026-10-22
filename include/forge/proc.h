#pragma once

#include "switch.h"

#ifdef __cplusplus
extern "C" {
#endif

Handle forge_proc_getHandle();
u64 forge_proc_getProgramId();
bool forge_proc_isMhxxJpn();

#ifdef __cplusplus
}
#endif
