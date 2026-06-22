import { useState, type ReactNode, type Ref } from "react";

export default function InfoPanel({
  id,
  type,
  ref,
  children,
}: {
  id: string;
  type: string;
  ref: Ref<HTMLElement>;
  children: ReactNode;
}) {
  const [selectedView, SetView] = useState(0);

  return (
    <>
      <div ref={ref} id={id} className="panel">
        <div className="title">
          <h3>{type} View</h3>
        </div>
        <div className="info-content">
          <div className="tab-bar">
            {type == "Stack" ? (
              <>
                <button
                  type="button"
                  className={selectedView == 0 ? "selected" : ""}
                  onClick={() => {
                    SetView(0);
                  }}
                >{`Algo Stack`}</button>
                <button
                  type="button"
                  className={selectedView == 1 ? "selected" : ""}
                  onClick={() => {
                    SetView(1);
                  }}
                >
                  Call Stack
                </button>{" "}
              </>
            ) : type == "Graph" ? (
              <>
                <button
                  type="button"
                  className={selectedView == 0 ? "selected" : ""}
                  onClick={() => {
                    SetView(0);
                  }}
                >
                  Adjacency Matrix/List
                </button>
                <button
                  type="button"
                  className={selectedView == 1 ? "selected" : ""}
                  onClick={() => {
                    SetView(1);
                  }}
                >
                  Algo State
                </button>{" "}
              </>
            ) : (
              ""
            )}
          </div>
          <div className="panel-subsection">
            {selectedView == 0 ? children : <p>In Development</p>}
          </div>
        </div>
      </div>
    </>
  );
}

