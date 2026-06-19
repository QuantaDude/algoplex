/*
 * This header is used to declare a contract between the typescript/ javascript
 * code and the C++ code. It acts as a single source of truth for the info panel
 * formats like the stack panel.
 *
 * */

// @stack DFS       node:int
#define STACK_FORMAT_DFS "{{\"node\":\"{}\"}}"
// @stack DFS_A     node:int       phase:string
#define STACK_FORMAT_DFS_A "{{\"node\":{},\"phase\":\"{}\"}}"

// Indirection macro so you can pass an algo name token
#define GET_STACK_FMT(algo) STACK_FORMAT_##algo
