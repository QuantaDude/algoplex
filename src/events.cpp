

#include "events.hpp"
void dispatchSceneEvent(EventDescriptor ed) {
  EM_ASM(
      {
        const action = $0;
        const target = $1;
        const id = $2;

        window.dispatchEvent(
            new CustomEvent("scene_event", {detail : {action, target, id}}));
      },
      static_cast<int>(ed.e_Action), static_cast<int>(ed.e_Target),
      static_cast<int>(ed.e_id));
}

void dispatchUIEvent(EventDescriptor ed, Vector2 pos, int cur_value) {
  EM_ASM(
      {
        const action = $0;
        const target = $1;
        const id = $2;
        const pos_x = $3;
        const pos_y = $4;
        const val = $5;
        window.dispatchEvent(new CustomEvent("ui_event", {
          detail : {action, target, id, pos : {x : pos_x, y : pos_y}, val}
        }));
      },
      static_cast<int>(ed.e_Action), static_cast<int>(ed.e_Target),
      static_cast<int>(ed.e_id), static_cast<int>(pos.x),
      static_cast<int>(pos.y), static_cast<int>(cur_value));
}
