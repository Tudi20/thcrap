/**
  * Touhou Community Reliant Automatic Patcher
  * Main DLL
  *
  * ----
  *
  * Import Address Table detour calls for the win32_utf8 functions.
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// *Not* a _mod_ function to ensure that this remains on the lowest level
THCRAP_API void win32_detour(void);

#ifdef __cplusplus
}
#endif
