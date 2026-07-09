import { lazy, Suspense, useEffect, useState } from "react";
const MonacoEditor = lazy(() => import("../monacoEditor/MonacoEditor.tsx"));

export default function CodePanel({ ref }: { ref: Ref<HTMLElement> }) {
  const [code, setCode] = useState<string>("");

  useEffect(() => {
    const controller = new AbortController();

    fetch(`${import.meta.env.BASE_URL}/dfs.py`, { signal: controller.signal })
      .then((response) => response.text())
      .then((data) => setCode(data))
      .catch((err) => {
        if (err.name !== "AbortError")
          console.error("Failed to load code:", err);
      });

    return () => controller.abort();
  }, []);

  return (
    <>
      <div id="code-panel" ref={ref}>
        <Suspense fallback={<h3>Loading...</h3>}>
          <MonacoEditor code={code} />
        </Suspense>
      </div>
    </>
  );
}

