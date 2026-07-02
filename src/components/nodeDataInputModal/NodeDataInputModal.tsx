import { useState, type KeyboardEvent, type Ref, type RefObject } from "react";

type Props = {
  loc: { x: number; y: number };
  initValue: number;
  onEnter: (value: number) => void;
  onEscape: () => void;
};

export default function NodeDataInputModal({
  loc,
  initValue,
  onEnter,
  onEscape,
}: Props) {
  const [value, setValue] = useState<number>(initValue);
  return (
    <div
      className="input-modal panel"
      style={{
        insetInlineStart: loc.x,
        insetBlockStart: loc.y,
      }}
    >
      <input
        type="number"
        name="node-data"
        value={value}
        onChange={(e) => {
          setValue(parseInt(e.target.value));
        }}
        onKeyUp={(e: KeyboardEvent) => {
          if (e.key == "Return" || e.key == "Enter") {
            onEnter(value);
          } else if (e.key == "Escape") {
            onEscape();
          }
        }}
      />
    </div>
  );
}

