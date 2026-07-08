import { lazy, Suspense } from "react";
const MonacoEditor = lazy(() => import("../monacoEditor/MonacoEditor.tsx"));

export default function CodePanel({ ref }: { ref: Ref<HTMLElement> }) {
  return (
    <>
      <div id="code-panel" ref={ref}>
        <Suspense fallback={<h3>Loading...</h3>}>
          <MonacoEditor code=""/>
        </Suspense>
      </div>
    </>
  );
}

