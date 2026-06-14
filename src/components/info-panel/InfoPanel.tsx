import type { ReactNode, Ref } from "react";

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
  return (
    <>
      <div ref={ref} id={id} className="panel">
        <div className="title">
          <h3>{`${type} View`}</h3>
        </div>
        <div className="panel-subsection">
          {children}
        </div>
      </div>
    </>
  );
}

