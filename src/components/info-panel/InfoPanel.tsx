export default function InfoPanel({
  id,
  ref,
}: {
  id: string;
  ref: Ref<HTMLElement>;
}) {
  return (
    <>
      <div ref={ref} id={id} className="panel">
        <div className="panel-subsection">
          <button type="button">test</button>
        </div>
      </div>
    </>
  );
}

