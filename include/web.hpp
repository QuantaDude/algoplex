#ifndef WEB_H
#define WEB_H

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#include <emscripten/em_types.h>
#ifdef __cplusplus
extern "C" {
#endif
void toggle_console(void) __attribute__((used))
__attribute__((visibility("default")));
void close_window(void);
void print_float(float string);
void print_string(const char *);

void set_mode(int, int);

char *list_all_files(const char *);

void toggle_console_wrapper(void);
void set_canvas_size_wrapper(int *, int *);
void print_console(const char *);
void print_console_float(float);
void close_window_wrapper();
em_str_callback_func onPreloadSuccess(const char *);
em_str_callback_func onPreloadError(const char *);

void list_files();

#ifdef __cplusplus
}
#endif

#endif
#endif
