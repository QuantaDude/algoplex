import { useState, type RefObject } from "react";
import type { MainModule } from "../../types/wasmmodule";

export default function Navbar(props: { wasmModule: RefObject<MainModule> }) {
  const [stepSpeed, setStepSpeed] = useState(1);
  return (
    <>
      <nav className="navbar">
        <div className="navbar-title">
          <h3>AlgoPlex</h3>
        </div>
        <div className="navbar-actions">
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
        </div>
        <div className="navbar-speed-controls">
          <label htmlFor="step_speed">Algo speed</label>
          <div className="controls-row">
          <input
            type="range"
            name="step_speed"
            value={stepSpeed}
            min="0.5"
            max="10"
            step="0.5"
            onChange={(e) => {
              setStepSpeed(parseFloat(e.target.value));
            }}
          />
          <input
            type="number"
            name="step_speed_val"
            value={stepSpeed}
            min="0.5"
            max="10"
            onChange={(e) => {
              setStepSpeed(parseFloat(e.target.value));
            }}
          />
        </div>
        </div>
      </nav>
    </>
  );
}

