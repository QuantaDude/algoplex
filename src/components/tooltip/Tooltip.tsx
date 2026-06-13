import { useEffect, useState, type RefObject } from "react";

export type TooltipPage = {
  title: string;
  text: string[];
  newLoc: boolean;
  tooltipTarget: RefObject<HTMLElement>;
  tooltipPos: { x: number; y: number };
  arrowRotation: number;
  arrowRelDistance: { x: number; y: number };
};
export default function Tooltip({ pages }: { pages: TooltipPage[] }) {
  const [currentPage, setPage] = useState(0);
  const page = pages[currentPage];

  const [tooltipPos, setTooltipPos] = useState({ x: 0, y: 0 });
  // const animKey = `${page.tooltipPos.x}-${page.tooltipPos.y}`;
  const animKey = `${tooltipPos.x}-${tooltipPos.y}`;
  const [nudgeDir, setNudgeDir] = useState({ x: 0, y: 0 });
  const [ready, setReady] = useState(false);

  function hide() {
    setReady(false);
  }
  useEffect(() => {
    setReady(false);

    console.log(page.tooltipTarget.current.getBoundingClientRect().bottom);
    function setPos() {
      if (!page.tooltipTarget.current) return;
      const rect = page.tooltipTarget.current.getBoundingClientRect();

      const targetCX = (rect.right + rect.left) / 2;
      const targetCY = (rect.top + rect.bottom) / 2;

      const tipX = targetCX + page.tooltipPos.x;
      const tipY = targetCY + page.tooltipPos.y;

      // vector from tooltip to target
      const dx = targetCX - tipX;
      const dy = targetCY - tipY;
      const len = Math.sqrt(dx * dx + dy * dy);

      // normalize and scale to nudge distance in px
      const nudgeAmount = 6;
      setNudgeDir({
        x: (dx / len) * nudgeAmount,
        y: (dy / len) * nudgeAmount,
      });

      setTooltipPos({ x: tipX, y: tipY });
    }
    const id = setTimeout(() => {
      if (!page.tooltipTarget.current) return;
      setPos();
      window.addEventListener("resize", setPos);
      setReady(true); // show after position is calculated
    }, 0);

    return () => {
      clearTimeout(id);
      window.removeEventListener("resize", setPos);
    };
  }, [page]);
  return (
    <div
      className="tooltip panel"
      key={animKey}
      style={
        {
          insetInlineStart: tooltipPos.x,
          insetBlockStart: tooltipPos.y,
          "--tooltip-nudge-x": `${nudgeDir.x}px`,
          "--tooltip-nudge-y": `${nudgeDir.y}px`,
          visibility: ready ? "visible" : "hidden",
        } as React.CSSProperties
      }
    >
      <div
        className="tooltip-arrow"
        style={{
          insetInlineStart: `${page.arrowRelDistance.x}rem`,
          insetBlockStart: `${page.arrowRelDistance.y}rem`,
        }}
      >
        <svg
          viewBox="-4 0 8 12"
          width="16"
          height="24"
          xmlns="http://www.w3.org/2000/svg"
          style={
            {
              "--arrow-rotation": `${page.arrowRotation}deg`,
              "--arrow-nudge-x": `${nudgeDir.x}px`,
              "--arrow-nudge-y": `${nudgeDir.y}px`,
            } as React.CSSProperties
          }
        >
          <path d="M 0 0 L -1 8 L -4 8 L 0 12 L 4 8 L 2 8 L 1 8 L 0 0" />
        </svg>{" "}
      </div>

      <div className="title">
        <h3>Welcome Tour </h3>
      </div>
      <div className="tooltip-info">
        <h3>{pages[currentPage].title}</h3>
        {pages[currentPage].text.map((para, i) => {
          return <p key={i}>{para}</p>;
        })}
      </div>

      <div className="tooltip-controls">
        <button
          type="button"
          disabled={currentPage === 0}
          onClick={() => {
           setReady(false);
            setPage((p) => p - 1);
          }}
        >
          prev
        </button>
        <p>{`${currentPage + 1}/${pages.length}`} </p>
        <button
          type="button"
          disabled={currentPage == pages.length - 1}
          onClick={() => {
            setReady(false);
            setPage((p) => p + 1);
          }}
        >
          next
        </button>
      </div>
    </div>
  );
}

