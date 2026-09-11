# Depth First Search (basic)
#
# `graph` is provided by the AlgoPlex runtime — you don't create it yourself

"""
graph.nodes       -> {id: Node}
graph.adjacency   -> {id: [id, ...]}
graph.root_id     -> int
graph.stack       -> Stack
visited = set()
await dfs(graph, graph.root_id, visited)
"""


def main(graph):
    visited = set()
    dfs(graph, graph.root_id, visited)


def dfs(graph, node_id, visited):
    if node_id in visited:
        return
    visited.add(node_id)

    graph.stack.push(node_id)
    graph.set_current(node_id)
    graph.mark_visited(node_id)

    print(f"visit {node_id}")

    for neighbor_id in graph.neighbors(node_id):
        if neighbor_id not in visited:
            dfs(graph, neighbor_id, visited)

    graph.stack.pop()
