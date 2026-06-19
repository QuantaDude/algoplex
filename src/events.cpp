

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
