import { useState, type Ref } from "react";

export default function AlgoMenuPanel({ ref }: {ref: Ref<HTMLElement>}) {
  const [isOpen, setIsOpen] = useState(false);
  return (
    <aside ref={ref} className="sidebar panel">
      <div className="title">
        <h3>Algorithm List</h3>
      </div>
      <div className="panel-subsection">
        <div id="algo-list">
          <button
            type="button"
            className="dropdown-btn"
            onClick={(e) => {
              setIsOpen(!isOpen);
            }}
          >
            Graph/Tree{" "}
            <svg
              className={`arrow ${isOpen ? "open" : ""}`}
              width="16"
              height="10"
              fill="currentColor"
              viewBox="0 0 8 5"
            >
              <path d="M 0 0 L 4 5 L 8 0" />
            </svg>
          </button>
          <ul className={`dropdown-li ${isOpen ? "open" : ""}`}>
            <li>
              <button className="dropdown-li-btn">DFS</button>
            </li>
            <li>
              <button className="dropdown-li-btn">DFS - Advanced</button>
            </li>
          </ul>
        </div>
      </div>
    </aside>
  );
}

