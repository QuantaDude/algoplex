import type { EventAction, EventTarget } from "./events";

export type EventDescriptor = {
  action: EventAction;
  target: EventTarget;
  id: number;
};

