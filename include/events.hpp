
#pragma once
#include <emscripten/em_asm.h>
#include <emscripten/emscripten.h>
#include <limits>
#include <sys/types.h>
enum class EventTarget : u_int8_t {
  Node,
  Edge,
  Bar,
  Stack,
  Queue,
};

enum class EventAction : u_int8_t {
  Add,
  Remove,
  Edit,
  AlgoStateUpdate,
};

struct EventDescriptor {
  EventAction e_Action;
  EventTarget e_Target;

  u_int32_t e_id = std::numeric_limits<u_int32_t>::max();

  EventDescriptor(EventAction e_A, EventTarget e_T,
                  u_int32_t id = std::numeric_limits<u_int32_t>::max())
      : e_Action(e_A), e_Target(e_T), e_id(id) {}
};

EventDescriptor createEvent(EventAction action, EventTarget target,
                            const char *id = nullptr);

void dispatchSceneEvent(EventDescriptor ed);
