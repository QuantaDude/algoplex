import type { RefObject } from "react";
import type { MainModule } from "../../types/wasmmodule";

export default function Navbar(props: { wasmModule: RefObject<MainModule> }) {
  return (
    <>
      <nav className="navbar">
        <button
          type="button"
          onClick={(e) => {
            const ptr: Number = props.wasmModule.current._get_scene_ptr();
            props.wasmModule.current._start_algo(ptr);
          }}
        >
          Start
        </button>
        <button
          type="button"
          onClick={(e) => {
            const ptr = props.wasmModule.current._get_scene_ptr();

            props.wasmModule.current._step_algo(ptr);
          }}
        >
          Step
        </button>
        <button type="button">Pause</button>
        <button type="button">Stop</button>
      </nav>
    </>
  );
}

