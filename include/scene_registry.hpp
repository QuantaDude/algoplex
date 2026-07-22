#pragma once
#include <cstdint>
#include <format>
#include <stdint.h>
#include <sys/types.h>
enum class SceneType {
  Graph, // Graph & Tree
  Sort,
};

enum class AlgorithmId {
  //Graph algos
  DFS,
  DFS_A,
  BFS,

  //sorting alsogs
  Bubble,

  ALGORITHM_COUNT,
};

struct AlgorithmInfo {

  const AlgorithmId id;
  const char *key;
  const char *name;
  const SceneType category;
  const char *subCategories;
};

static const AlgorithmInfo ALGORITHMS[] = {
    {AlgorithmId::DFS, "dfs", "Depth First Search (Basic)", SceneType::Graph,
     "searching, traversal"},
    {AlgorithmId::DFS_A, "dfs_a", "Depth First Search (Advanced)",
     SceneType::Graph, "searching, traversal"},
    {AlgorithmId::BFS, "bfs", "Breadth First Search", SceneType::Graph,
     "searching, traversal"},
    {AlgorithmId::Bubble, "bubble_sort", "Bubble Sort", SceneType::Sort,
     "sorting"}};

static constexpr uint16_t ALGORITHM_COUNT =
    sizeof(ALGORITHMS) / sizeof(ALGORITHMS[0]);

static_assert(ALGORITHM_COUNT == static_cast<int>(AlgorithmId::ALGORITHM_COUNT),
              "Algorithm Count mismatch.");
