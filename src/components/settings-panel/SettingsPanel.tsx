import { useEffect, useState, type Ref, type RefObject } from "react";
import type { MainModule } from "../../types/wasmmodule";

export default function SettingsPanel({
  wasmModule,
  ref,
}: {
  wasmModule: RefObject<MainModule>;
  ref: Ref<HTMLElement>;
}) {
  const [majorMode, setMajorMode] = useState(0);
  const [minorMode, setMinorMode] = useState(0);

  useEffect(() => {
    window.setMode = (major: number, minor: number) => {
      setMajorMode(major);
      setMinorMode(minor);
    };
  }, []);
  return (
    <aside ref={ref} className="right panel">
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
                const ptr = wasmModule.current._get_scene_ptr();
                console.log(ptr);
                wasmModule.current._update_mode(ptr, b.id, minorMode);
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
                const ptr = wasmModule.current._get_scene_ptr();
                wasmModule.current._update_mode(ptr, majorMode, b.id);
              }}
              disabled={majorMode === 0 ? true : false}
            >
              {b.name}
            </button>
          );
        })}
      </div>
    </aside>
  );
}

