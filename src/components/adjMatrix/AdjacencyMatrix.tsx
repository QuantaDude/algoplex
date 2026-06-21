import { Fragment, useEffect, useState, type RefObject } from "react";
import type { DFS_AAdjMatFrame } from "../../types/InfoPanel";
import type { EventDescriptor } from "../../types/EventDescriptor";
import { EventAction, EventTarget } from "../../types/events";
import type { MainModule } from "../../types/wasmmodule";

export default function AdjacencyMatrix({
  wasmModule,
}: {
  wasmModule: RefObject<MainModule>;
}) {
  const [nodes, setNodes] = useState<DFS_AAdjMatFrame[]>([]);

  useEffect(() => {
    const handler = (e: CustomEvent) => {
      console.log(e.detail);
      const event: EventDescriptor = e.detail;
      if (
        event.target != EventTarget.Queue &&
        event.target != EventTarget.Stack
      ) {
        switch (event.action) {
          case EventAction.Add:
          case EventAction.Edit:
          case EventAction.Remove:
            //fetch nodes
            //
            const ptr = wasmModule.current._get_adj_json();
            const json = wasmModule.current.UTF8ToString(ptr);

            const nodes: DFS_AAdjMatFrame[] = JSON.parse(json);
            setNodes(nodes);
            break;
          default:
            break;
        }
      }
    };
    window.addEventListener("scene_event", handler as EventListener, {
      once: false,
    });

    return () =>
      window.removeEventListener("scene_event", handler as EventListener);
  }, []);
  function hasEdge(node1: number, node2: number) {
    if (nodes[node1].edges.includes(node2)) return true;
    else if (nodes[node2].edges.includes(node1)) return true;
    else return false;
  }
  return (
    <div
      className="adjacency-matrix"
      style={{ "--node-count": nodes.length } as React.CSSProperties}
    >
      <div className="matrix-corner" />

      {nodes.map((node) => (
        <div className="matrix-col-label" key={`${node.node}-col`}>
          {node.node}
        </div>
      ))}
      {nodes.map((row) => (
        <Fragment key={`${row.node}-row`}>
          <div className="matrix-row-label" >
            {row.node}
          </div>

          {nodes.map((col) => (
            <div
              key={`${row.node}-${col.node}-edge`}
              className={`matrix-cell ${hasEdge(row.node, col.node) ? "matrix-cell--active" : ""}`}
            ></div>
          ))}
        </Fragment>
      ))}
    </div>
  );
}

