
enum class EventType {
  Add = 0,
  Remove = 3,
  Edit = 6,
};
enum class Event {
  NodeAdd,
  EdgeAdd,
  BarAdd,

  NodeRemove,
  EdgeRemove,
  BarRemove,

  NodeEdit,
  EdgeEdit,
  BarEdit,

  EVENTS_COUNT
};

enum class Target { Node = 0,
  Edge = 1,
  Bar = 2 };

EventType getType(Event e) {
  return static_cast<EventType>((std::to_underlying(e) / 3) * 3);
}

Target getTarget(Event e) {
  return static_cast<Target>(std::to_underlying(e) % 3);
}

static_assert(std::to_underlying(Event::NodeRemove) ==
              std::to_underlying(EventType::Remove));
static_assert(std::to_underlying(Event::NodeEdit) ==
              std::to_underlying(EventType::Edit));
