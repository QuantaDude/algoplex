
#include <sys/types.h>

enum class AlgorithmType {
  Graph,
  Sort,
};

enum class AlgorithmId {
  DFS,
  BFS,
  Bubble,

  ALGORITHM_COUNT,
};

struct AlgorithmInfo {

  const AlgorithmId id;
  const char *key;
  const char *name;
  const AlgorithmType category;
  const char *subcategories;
};

static const AlgorithmInfo ALGORITHMS[] = {
    {AlgorithmId::DFS, "dfs", "Depth First Search", AlgorithmType::Graph,
     "searching, traversal"}

};

static constexpr int ALGORITHM_COUNT =
    sizeof(ALGORITHMS) / sizeof(ALGORITHMS[0]);

std::static_assert(ALGORITHM_COUNT ==
                       static_cast<int>(AlgorithmId::ALGORITHM_COUNT),
                   "Algorithm Count mismatch.");
