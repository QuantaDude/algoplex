import {
  useEffect,
  useRef,
  useState,
  type MouseEvent,
  type Ref,
  type RefObject,
} from "react";
import type { MainModule } from "../../types/wasmmodule";
import type { GraphNodeListFrame } from "../../types/InfoPanel";
import { EventAction } from "../../types/events";

export default function SettingsPanel({
  wasmModule,
  ref,
}: {
  wasmModule: RefObject<MainModule>;
  ref: Ref<HTMLElement>;
}) {
  const [majorMode, setMajorMode] = useState(0);
  const [minorMode, setMinorMode] = useState(0);
  const [nodeList, setNodeList] = useState<GraphNodeListFrame[]>([]);
  const [rootNode, setRootNode] = useState<{ node: number; data: number }>({
    node: 0,
    data: 0,
  });
  const [nodeListVisibility, setNodeListVisibility] = useState(false);
  const nodeListMenuRef = useRef<HTMLElement>(null);

  function checkClickPos(m_x: number, m_y: number) {
    const rect = nodeListMenuRef.current?.getBoundingClientRect();
    if (nodeListVisibility) {
      if (
        m_x >= rect?.x &&
        m_x < rect?.width &&
        m_y >= rect.y &&
        m_y < rect?.height
      ) {
        return true;
      }
      return false;
    }
    // nodeList is hidden so return true. Otherwise the list would never open
    return true;
  }
  useEffect(() => {
    window.setMode = (major: number, minor: number) => {
      setMajorMode(major);
      setMinorMode(minor);
    };

    const handler = (e: CustomEvent) => {
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
            const ptr = wasmModule.current._get_node_list_json();
            const json = wasmModule.current.UTF8ToString(ptr);

            const root_node_ptr = wasmModule.current._get_root_node_json();
            const root_node_json =
              wasmModule.current.UTF8ToString(root_node_ptr);

            const root_node = JSON.parse(root_node_json);
            const nodes: GraphNodeListFrame[] = JSON.parse(json);

            setRootNode(root_node);
            setNodeList(nodes);
            break;
          default:
            break;
        }
      }
    };
    window.addEventListener("scene_event", handler as EventListener);
  }, []);
  return (
    <aside
      ref={ref}
      className="right panel"
      onClick={(e: MouseEvent) => {
        if (!checkClickPos(e.clientX, e.clientY)) {
          setNodeListVisibility(false);
          wasmModule.current._set_hover_state(false, 0);
        }
      }}
    >
      <div className="title">
        <h3>Controls & Settings</h3>
      </div>
      <div className="panel-subsection">
        {[
          { id: 0, name: "Free" },
          { id: 1, name: "Node" },
          { id: 2, name: "Edge" },
        ].map((b) => {
          return (
            <button
              type="button"
              key={b.id}
              className={majorMode === b.id ? "selected" : ""}
              onClick={(e) => {
                setMajorMode(b.id);
                wasmModule.current._update_mode(b.id, minorMode);
              }}
            >
              {b.name}
            </button>
          );
        })}
      </div>
      <div className="panel-subsection">
        {[
          { id: 0, name: "Select" },
          { id: 1, name: "Create" },
          { id: 2, name: "Edit" },
        ].map((b) => {
          return (
            <button
              type="button"
              key={b.id}
              className={minorMode === b.id && majorMode != 0 ? "selected" : ""}
              onClick={(e) => {
                setMinorMode(b.id);
                wasmModule.current._update_mode(majorMode, b.id);
              }}
              disabled={majorMode === 0 ? true : false}
            >
              {b.name}
            </button>
          );
        })}
      </div>

      <br />
      <br />
      <br />
      <div className="panel-subsection">
        <div className="settings-select-root">
          <label htmlFor="set-root">Root node</label>
          <div id="node-list">
            <button
              type="button"
              className="dropdown-btn"
              onClick={() => {
                setNodeListVisibility(!nodeListVisibility);
                wasmModule.current._save_camera_pos();
              }}
            >
              {`${rootNode.data}(${rootNode.node})`}
              <svg
                className={`arrow ${!!nodeListVisibility ? "open" : ""}`}
                width="16"
                height="10"
                fill="currentColor"
                viewBox="0 0 8 5"
              >
                <path d="M 0 0 L 4 5 L 8 0" />
              </svg>
            </button>
            <ul
              ref={nodeListMenuRef}
              className={`dropdown-li  ${!!nodeListVisibility ? "open" : ""}`}
            >
              {nodeList.map((node) => (
                <li key={node.node}>
                  <button
                    type="button"
                    className="dropdown-li-btn"
                    onMouseEnter={() => {
                      wasmModule.current._set_hover_state(true, node.node);
                    }}
                    onClick={() => {
                      wasmModule.current._set_root_node(node.node);
                      wasmModule.current._save_camera_pos();
                      wasmModule.current._set_hover_state(false, node.node);
                      setNodeListVisibility(!nodeListVisibility);
                    }}
                  >
                    {`${node.data}(${node.node})`}
                  </button>
                </li>
              ))}
            </ul>
          </div>
          <p>or</p>
          <button type="button" id="set-root">
            Select in canvas
          </button>
        </div>
      </div>

      <div className="panel-subsection panel-footer">
        <button
          type="button"
          onClick={() => {
            wasmModule.current._reset_scene();
          }}
        >
          Reset
        </button>
        <button
          type="button"
          onClick={() => {
            wasmModule.current._toggle_keybind_overlay();
          }}
        >
          Toggle Keybinds Overlay
        </button>
      </div>
    </aside>
  );
}

