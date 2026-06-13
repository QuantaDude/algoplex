export default function CodePanel({ ref }: { ref: Ref<HTMLElement> }) {
  return (
    <>
      <div id="code-panel" ref={ref}>
        <textarea rows="100%" cols="100%"></textarea>
      </div>
    </>
  );
}

