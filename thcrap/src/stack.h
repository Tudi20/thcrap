/**
  * Touhou Community Reliant Automatic Patcher
  * Main DLL
  *
  * ----
  *
  * Patch stack evaluators and information.
  */

#pragma once

// Iteration direction
typedef enum {
	SCI_BACKWARDS = -1,
	SCI_FORWARDS = 1
} sci_dir_t;

// Iteration state. [patch_info] and [fn] hold the current patch and chain
// file name after each call to stack_chain_iterate().
typedef struct {
	size_t chain_step;
	size_t chain_limit;
	size_t chain_reset;
	size_t patch_step;
	size_t patch_limit;
	const patch_t* patch_info;
	const char* fn;
} stack_chain_iterate_t;

typedef char** (*resolve_chain_t)(const char *fn);

/// File resolution
/// ---------------
// Creates a JSON array containing the the default resolving chain for [fn].
// The chain currently consists of [fn] itself, followed by its build-specific
// name returned by fn_for_build().
// All resolving functions that take a chain parameter (instead of a file
// name) should use the chain created by this function.
TH_CALLER_CLEANUP(chain_free)
THCRAP_API char **resolve_chain(const char *fn);

// Free a chain created by resolve_chain
THCRAP_API void chain_free(char **chain);

// Set a user-defined function used to create the chain returned by resolve_chain.
THCRAP_API void set_resolve_chain(resolve_chain_t function);

// Builds a chain for a game-local file name.
TH_CALLER_CLEANUP(chain_free)
THCRAP_API char **resolve_chain_game(const char *fn);

// Set a user-defined function used to create the chain returned by resolve_chain_game.
THCRAP_API void set_resolve_chain_game(resolve_chain_t function);

// Repeatedly iterate through the stack using the given resolving [chain].
// [sci] keeps the iteration state.
THCRAP_API bool TH_FASTCALL stack_chain_iterate(stack_chain_iterate_t *sci, char **chain, sci_dir_t direction);

// TODO: Update the documentation of this function to properly reflect the current structure
// Walks through the given patch stack, merging every file with the filename
// [fn] into a single JSON object.
// Returns the merged JSON object or NULL if there is no matching file
// in the patch stack.
// If given, [file_size] receives the maximum number of bytes required to store
// the final merged JSON data.
// If [patch] is NULL, the current patch stack is used instead.
THCRAP_API json_t* stack_json_resolve_chain(char **chain, size_t *file_size);
// Uses the current patch stack
THCRAP_API json_t* stack_json_resolve(const char *fn, size_t *file_size);

json_t* stack_json_resolve_vfs(const char* fn, size_t* file_size);

// Generic file resolver. Returns a stream of the file matching the [chain]
// with the highest priority inside the patch stack, or INVALID_HANDLE_VALUE
// if there is no such file in the stack.
THCRAP_API HANDLE stack_file_resolve_chain(char **chain);

// Searches the current patch stack for a replacement for the game data file
// [fn] and returns either a stream or a newly created buffer, analogous to
// file_stream() and file_stream_read().
THCRAP_API HANDLE stack_game_file_stream(const char *fn);
THCRAP_API void* stack_game_file_resolve(const char *fn, size_t *file_size);

// Resolves a game-local JSON file.
THCRAP_API json_t* stack_game_json_resolve(const char *fn, size_t *file_size);
/// ---------------

// Generic file name resolver. Returns the file name of the existing file
// matching the [chain] with the highest priority inside the patch stack.
TH_CALLER_FREE THCRAP_API char* stack_fn_resolve_chain(char **chain);

/// Information
/// -----------
// Displays a message box showing missing archives in the current patch stack,
// if there are any.
THCRAP_API void stack_show_missing(void);

// Shows the MOTD of each individual patch.
void stack_show_motds(void);

// Print the content of the stack to the logs
void stack_print(void);
/// -----------

/// Iteration and Manipulation
/// ------------
// Add a patch to the stack from a patch_t.
THCRAP_API void stack_add_patch(patch_t *patch);

// Add a patch to the stack from a json description.
THCRAP_API void stack_add_patch_from_json(json_t *patch);

// Remove the patch patch_id from the stack.
THCRAP_API void stack_remove_patch(const char *patch_id);

// Get the number of patches in the stack
THCRAP_API size_t stack_get_size();

// Iterate over the patches in the stack
THCRAP_API void stack_foreach(void (*callback)(const patch_t *patch, void *userdata), void *userdata);

#ifdef __cplusplus
extern "C++" {

// Alternative version of stack_foreach that support a C++ lambda as parameter
THCRAP_API void stack_foreach_cpp(std::function<void (const patch_t*)> callback);

}
#endif

// Returns:
// •  0 if the current patch stack contains a patch with the given name, and
//      if that patch in turn contains a .js file or directory for the current
//      game.
// •  1 if there is such a patch, but no .js file or directory for the current
//      game. This patch is then removed from the stack.
// • -1 if the current patch stack contains no patch with the given name.
//
// DLLs that add game support should call this in their thcrap_plugin_init()
// function with their corresponding base patch, i.e., the name of the patch
// that would contain that DLL if we had patch-level plugins yet. If this
// function returns a nonzero value, thcrap_plugin_init() can then itself
// return a nonzero value to disable that DLL, since thcrap is not patching
// any of the games it supports.
// This avoids the need for hardcoding a list of supported games in the patch
// DLL itself, when there is already a patch with JSON files that fulfill
// essentially the same function.
//
// TODO: Only intended as a stopgap measure until we actually have patch-level
// plugins, or a completely different solution that avoids having every base_*
// patch ever as an explicit, upfront dependency of every stack ever.
THCRAP_API int stack_remove_if_unneeded(const char *patch_id);

THCRAP_API int stack_check_if_unneeded(const char* patch_id);

// Clear the stack and free its patches
THCRAP_API void stack_free();
/// ------------
