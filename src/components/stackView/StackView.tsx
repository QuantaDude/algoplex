import { useEffect, useRef, useState, type RefObject } from "react";
import { EventActionLabel, EventTargetLabel } from "../../types/events";
import type { MainModule, WasmModule } from "../../types/wasmmodule";
import type { DFS_AStackFrame } from "../../types/InfoPanel";
import type { EventDescriptor } from "../../types/EventDescriptor";

export default function StackView({
  items,
  onUpdate,
  wasmModule,
}: {
  items: DFS_AStackFrame[];
  onUpdate: (stack: Array<DFS_AStackFrame>) => void;
  wasmModule: RefObject<MainModule>;
}) {
  const prevItems: RefObject<DFS_AStackFrame[]> = useRef<DFS_AStackFrame[]>([]);
  const [newItems, SetNewItems] = useState<DFS_AStackFrame[]>([]);
  useEffect(() => {
    const handler = (e: CustomEvent) => {
      const event: EventDescriptor = e.detail;
      if (EventActionLabel[event.action] === EventActionLabel[3]) {
        if (EventTargetLabel[event.target] === EventTargetLabel[3]) {
          const ptr = wasmModule.current._get_stack_json();
          const json = wasmModule.current.UTF8ToString(ptr);
          const stack: DFS_AStackFrame[] = JSON.parse(json);
          onUpdate(stack);
        }
      }
    };

    window.addEventListener("scene_event", handler as EventListener);
    return () =>
      window.removeEventListener("scene_event", handler as EventListener);
  }, []);
  useEffect(() => {
    const prevSet = new Set(
      prevItems.current.map((f) => `${f.node}${f.phase}`),
    );
    const newlyAdded = items.filter(
      (item) => !prevSet.has(`${item.node}${item.phase}`),
    );

    if (newlyAdded.length > 0) {
      SetNewItems(newlyAdded);
    }
    prevItems.current = items;
    // items.map((itemc> => {
    //   prevSet.has(item) ? undefined : SetNewItems([...newItems, item]);
    //   prevItems.current.push(...newItems);
    // });
  }, [items]);

  return (
    <>
      <div className="stack-view">
        {items.map((item, index) => (
          <p
            key={index}
            className={
              +newItems.includes(item)
                ? "stack-cell stack-new-element"
                : "stack-cell stack-element"
            }
          >
            {item.node} {item.phase}
          </p>
        ))}
      </div>
    </>
  );
}

