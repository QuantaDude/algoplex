/*
 * This header is used to declare a contract between the typescript/ javascript
 * code and the C++ code. It acts as a single source of truth for the info panel
 * formats like the stack panel.
 *
 * */

// @stack             DFS                 node:int
#define STACK_FORMAT_DFS "{{\"node\":\"{}\"}}"
// @stack             DFS_A               node:int       phase:string
#define STACK_FORMAT_DFS_A "{{\"node\":{},\"phase\":\"{}\"}}"

// @AdjacencyMatrix   DFS_A               node:int        edges:int[]
#define ADJ_MAT_FMT_DFS_A "{{\"node\":{},\"edges\":[{}]}}"

// Indirection macro so you can pass an algo name token
#define GET_STACK_FMT(algo) STACK_FORMAT_##algo
#define GET_ADJ_MAT_FMT(algo) ADJ_MAT_FMT_##algo
