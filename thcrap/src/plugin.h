/**
  * Touhou Community Reliant Automatic Patcher
  * Main DLL
  *
  * ----
  *
  * Plug-in and module handling.
  */

#pragma once

/**
  * To be identified as such, every thcrap plugin must export a function named
  *
  * int TH_STDCALL thcrap_plugin_init()
  *
  * which should return 0 on success, and anything else if the plugin should
  * be removed. This function is called directly after the plugin was loaded
  * via LoadLibrary().
  */

// Returns a pointer to a function with the given name in the list of exported
// functions. Basically a GetProcAddress across the engine and all plug-ins.
THCRAP_API uintptr_t func_get(const char *name);
THCRAP_API uintptr_t func_get_len(const char *name, size_t name_len);

// Adds a pointer to a function to the list of functions used by func_get
THCRAP_API int func_add(const char *name, uintptr_t addr);

// Removes a function from the list of functions used by func_get
// This function is nessesairy for plugins to be able to unload themselves
THCRAP_API bool func_remove(const char *name);

/// Module functions
/// ================
/**
  * If the name of a function exported by any thcrap DLL matches the pattern
  * "*_mod_[suffix]", it is automatically executed when calling
  * mod_func_run() with [suffix]. The module hooks currently supported by
  * the thcrap core, with their parameter, include:
  *
  * • "init" (NULL)
  *   Called after a DLL has been loaded.
  *
  * • "detour" (NULL)
  *   Called after a DLL has been loaded. If a module needs to add any new
  *   detours, it should implement this hook, using one or more calls to
  *   detour_cache_add().
  *
  * • "post_init" (NULL)
  *   Called after the initialization of thcrap and all modules is complete.
  *
  * • "repatch" (const char* files_changed[])
  *   Called when files from a patch have been changed outside the game and
  *   need to be reloaded. [files_changed] contains the full path of each
  *   changed file and is terminated with an extra NULL pointer.
  *
  * • "thread_exit" (NULL)
  *   Called whenever a thread in the process exits (DLL_THREAD_DETACH in
  *   DllMain()).
  *
  * • "exit" (NULL)
  *   Called when shutting down the process.
  */

// Module function type.
typedef void (TH_CDECL *mod_call_type)(void *param);

// Removes a module hook function from the unordered map of module hook function
// This function is nessesairy for plugins to be able to unload themselves
THCRAP_API void mod_func_remove(const char *pattern, mod_call_type func);

// Removes a patch hook function from the unordered map of patch hook function
THCRAP_API void patch_func_remove(const char *pattern, mod_call_type func);

#ifdef __cplusplus
extern "C++" {

class mod_funcs_t : public std::unordered_map<std::string_view, std::vector<mod_call_type>> {
public:
	// Builds an unordered map mapping the suffixes of all module hook functions
	// occurring in [funcs] to an array of pointers to all the functions in
	// [funcs] with that suffix:
	// {
	//	"suffix": [
	//		<function pointer>. <function pointer>. ...
	//	],
	//	...
	// }
	inline void build(exported_func_t* funcs, std::string_view infix);

	// Runs every module hook function for [suffix] in [mod_funcs]. The execution
	// order of the hook functions follows the order their DLLs were originally
	// loaded in, but is undefined within the functions of a single DLL.
	inline void run(std::string_view suffix, void* param);

	inline void remove(std::string_view suffix, mod_call_type func);
};

}
#endif

// Calls mod_fun_run() with all registered functions from all thcrap DLLs.
void mod_func_run_all(const char *suffix, void *param);

// Calls mod_fun_run() with all registered functions from patches.
THCRAP_API void patch_func_run_all(const char *pattern, void *param);
/// ===================

// Initializes a plug-in DLL at [hMod]. This means registering all of its
// exports, and calling its "init" and "detour" module functions.
int plugin_init(HMODULE hMod);

int patch_func_init(exported_func_t *funcs_new);

// Loads all thcrap plugins from the given directory.
THCRAP_INTERNAL_API int plugins_load(const char *dir);

THCRAP_INTERNAL_API int plugins_close(void);
