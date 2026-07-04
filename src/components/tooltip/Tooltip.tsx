import { useEffect, useRef, useState, type RefObject } from "react";
import useWindowSize from "../hooks/useWindowResize";

export type TooltipPage = {
  title: string;
  text: string[];
  newLoc: boolean;
  tooltipTarget: RefObject<HTMLElement>;
  targetCOverride?: {
    right: number;
    left: number;
    top: number;
    bottom: number;
    x: number;
    y: number;
    width: number;
    height: number;
  };
  subHighlightTarget?: RefObject<HTMLElement>;
  tooltipPos: { x: number; y: number };
  arrowRotation: number;
  arrowRelDistance: { x: number; y: number };
};
export default function Tooltip({
  onClose,
  pages,
}: {
  onClose: () => void;
  pages: TooltipPage[];
}) {
  const [currentPage, setPage] = useState(0);
  const page = pages[currentPage];
  const prevTarget = useRef<RefObject<HTMLElement> | null>(null);
  const animKey = useRef(0);
  const [visible, setVisible] = useState(false);

  // useWindowSize();
  // --- Compute everything during render ---
  const locationChanged = prevTarget.current !== page.tooltipTarget;
  if (locationChanged) {
    animKey.current += 1;
    prevTarget.current = page.tooltipTarget;
  }

  let tooltipPos = { x: 0, y: 0 };
  let nudgeDir = { x: 0, y: 0 };

  // if (page.tooltipTarget.current) {
  const rect = page.tooltipTarget.current.getBoundingClientRect();
  const f_rx =
    page.targetCOverride != null
      ? rect.left * page.targetCOverride.left +
        rect.right * page.targetCOverride.right
      : rect.left + rect.right;

  const f_ry =
    page.targetCOverride != null
      ? rect.top * page.targetCOverride.top +
        rect.bottom * page.targetCOverride.bottom
      : rect.top + rect.bottom;

  const targetCX = f_rx / 2;
  const targetCY = f_ry / 2;

  const tipX = targetCX + page.tooltipPos.x;
  const tipY = targetCY + page.tooltipPos.y;
  const dx = targetCX - tipX;
  const dy = targetCY - tipY;
  const len = Math.sqrt(dx * dx + dy * dy);
  const nudgeAmount = 6;
  tooltipPos = { x: tipX, y: tipY };
  nudgeDir = { x: (dx / len) * nudgeAmount, y: (dy / len) * nudgeAmount };
  // }

  useEffect(() => {
    if (locationChanged) setVisible(false);

    const id = setTimeout(() => {
      if (!page.tooltipTarget.current) return;
      setVisible(true);
    }, 0);

    return () => clearTimeout(id);
  }, [currentPage]);

  return (
    <>
      {page.tooltipTarget.current != null && (
        <div
          key={`${animKey.current}-highlight`}
          className="tooltip-target-highlight"
          style={{
            visibility: visible ? "visible" : "hidden",
            insetInlineStart:
              page.tooltipTarget.current.getBoundingClientRect().x,
            insetBlockStart:
              page.tooltipTarget.current.getBoundingClientRect().y,
            width: page.tooltipTarget.current.getBoundingClientRect().width,
            height: page.tooltipTarget.current.getBoundingClientRect().height,
            zIndex: 1001,
          }}
        ></div>
      )}
      {page.subHighlightTarget?.current != null && (
        <div
          key={`${animKey.current}-highlight-2`}
          className="tooltip-target-highlight"
          style={{
            visibility: visible ? "visible" : "hidden",
            insetInlineStart:
              page.subHighlightTarget.current.getBoundingClientRect().x,
            insetBlockStart:
              page.subHighlightTarget.current.getBoundingClientRect().y,
            width:
              page.subHighlightTarget.current.getBoundingClientRect().width,
            height:
              page.subHighlightTarget.current.getBoundingClientRect().height,
            zIndex: 1001,
          }}
        ></div>
      )}

      <div
        className="tooltip panel"
        key={animKey.current}
        style={
          {
            insetInlineStart: tooltipPos.x,
            insetBlockStart: tooltipPos.y,
            "--tooltip-nudge-x": `${nudgeDir.x}px`,
            "--tooltip-nudge-y": `${nudgeDir.y}px`,
            visibility: visible ? "visible" : "hidden",
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
          <button type="button" onClick={() => onClose()}>
            &#x2716;
          </button>
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
              setPage((p) => p - 1);
            }}
          >
            prev
          </button>
          <p>{`${currentPage + 1}/${pages.length}`} </p>
          <button
            type="button"
            // disabled={currentPage == pages.length - 1}
            onClick={() => {
              currentPage != pages.length - 1
                ? setPage((p) => p + 1)
                : onClose();
            }}
          >
            {currentPage == pages.length - 1 ? "\u2716" : "next"}
          </button>
        </div>
      </div>
    </>
  );
}

