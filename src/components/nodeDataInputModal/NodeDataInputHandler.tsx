import { useEffect, useRef, useState, type RefObject } from "react";
import { EventTarget, EventAction } from "../../types/events";
import type { EventDescriptor } from "../../types/EventDescriptor";
import NodeDataInputModal from "./NodeDataInputModal";
import type { MainModule } from "../../types/wasmmodule";
type Props = {
  wasmModule: RefObject<MainModule>;
  canvasRef: RefObject<HTMLElement>;
};

export function NodeDataInputHandler({ wasmModule, canvasRef }: Props) {
  const [inputModal, showInputModal] = useState(false);
  const loc = useRef({
    x: canvasRef.current.getBoundingClientRect().x,
    y: canvasRef.current.getBoundingClientRect().y,
  });

  const nodeId: RefObject<number> = useRef(Number.MAX_VALUE);
  const initValue = useRef(0);

  function sendValue(value: number) {
    wasmModule.current._set_node_val(nodeId.current, value);
    showInputModal(false);
  }
  useEffect(() => {
    const handler = (e: CustomEvent) => {
      const event: EventDescriptor = e.detail;
      if (event.target == EventTarget.Node) {
        switch (event.action) {
          case EventAction.Edit:
            console.log(e.detail);
            nodeId.current = e.detail.id;

            initValue.current = e.detail.val;

            const canvas = canvasRef.current as HTMLCanvasElement;
            const rect = canvas.getBoundingClientRect();
            const scaleX = rect.width / canvas.width;
            const scaleY = rect.height / canvas.height;
            loc.current = {
              x: rect.left + e.detail.pos.x * scaleX,
              y: rect.top + e.detail.pos.y * scaleY,
            };

            showInputModal(true);
            break;
          default:
            break;
        }
      }
    };

    window.addEventListener("ui_event", handler as EventListener);
  }, []);

  return (
    inputModal && (
      <NodeDataInputModal
        loc={loc.current}
        initValue={initValue.current}
        onEnter={sendValue}
        onEscape={()=> {showInputModal(false)}}
      />
    )
  );
}

