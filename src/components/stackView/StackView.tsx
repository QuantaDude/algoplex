import { useEffect, useRef, useState, type RefObject } from "react";

export default function StackView({ items }: { items: string[] }) {
  const prevItems: RefObject<String[]> = useRef<String[]>([]);
  const [newItems, SetNewItems] = useState<String[]>([]);

  useEffect(() => {
    const prevSet = new Set(prevItems.current);
    const newlyAdded = items.filter((item) => !prevSet.has(item));

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
        {items.toReversed().map((item, index) => (
          <p
            key={index}
            className={
              newItems.includes(item) ? "stack-new-element" : "stack-element"
            }
          >
            {item}
          </p>
        ))}
      </div>
    </>
  );
}

