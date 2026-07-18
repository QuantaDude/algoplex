#pragma once
#include "app.hpp"
#include "colors.h"
#include "debug_panel_fmt.hpp"
#include "events.hpp"
#include "raylib.h"
#include "scene_registry.hpp"
#include "state.hpp"
#include <cstddef>
#include <cstdint>
#include <stack>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
#if defined(PLATFORM_WEB)
#import <emscripten.h>
#include <emscripten/em_macros.h>
#import <emscripten/em_types.h>
#endif // PLATOFRM_WEB

class GraphScene : public State {
  AlgorithmId a_id;

public:
  enum class DFSPhase {
    ENTER,
    EXIT
  };

  RenderTexture2D target;
  bool update_res = false;
  bool show_key_overlay = true;
  struct DFSFrame {
    u_int32_t node;
    DFSPhase phase;
  };
  inline const char *dfsPhaseToString(DFSPhase phase) {
    switch (phase) {
    case DFSPhase::ENTER:
      return "ENTER";
    case DFSPhase::EXIT:
      return "EXIT";
    default:
      return "UNKNOWN";
    }
  }
  AlgorithmState algorithm_state;
  Camera2D g_camera;
  Vector2 camera_old_offset;
  float camera_old_zoom;
  Vector2 camera_old_target;
  Vector2 mouse_world_pos;
  Font *m_font;

  // void GuiAlgoViz(BaseGuiState *state);

  enum class InteractionMode {
    None = 0, // can hover and delete
    NodeSelect = 1,
    EdgeSelect = 2,

    NodeCreate = 3,
    NodeEdit = 4,

    EdgeCreate = 5,
    EdgeEdit = 6,
    EdgeNONSENSE = 7
  } m_input_mode;
  int main_mode = 0; // 0=FREE, 1=NODE, 2=EDGE
  int sub_mode = 0;  // 0=SELECT, 1=CREATE, 2=EDIT
  struct Edge {
    size_t from;
    size_t to;
  };

  struct Node {
    u_int32_t id;

    Vector2 pos;
    Vector2 oldPos;
    uint16_t radius;
    Rectangle collider;

    int64_t data;
    std::vector<int> edges;
  };

  // Node *root;
  u_int32_t root_id;
  std::unordered_map<u_int32_t, size_t> id_to_node_idx;
  std::vector<Node> nodes;
  std::vector<Edge> edges;

  std::stack<DFSFrame> dfs_stack;

  std::vector<bool> visited;

  //NOTE: Refactor these iterators to ids
  std::vector<Node>::iterator selected_node;
  std::vector<Node>::iterator selected_edge_origin;

  bool moveCamera = false;
  size_t hoveredEdgeIdx = SIZE_MAX;
  size_t hoveredNodeIdx = SIZE_MAX;

  GraphScene(Font *);
  void init() override;
  void input() override;
  void update_input_mode();
  void draw(IVector2 *) override;
  void drawUI(IVector2);
  void update(IVector2 *resolution) override;
  bool IsMouseHoveringEdge(const Vector2 &, const Vector2 &, const Vector2 &,
                           float thickness = 5.0f);

  void dfs();
  void createStack();

  void resetScene();
  //updates the camera position when the mouse is hovring over an UI element
  void gotoNode(IVector2 *resolution);
  void gotoPos(IVector2 *resolution);
  int getCurrentAlgoId();

  void ToggleKeybindOverlay();
  void setRootNode(u_int32_t);
  void setNodeVal(u_int32_t, int);

  const char *getStackJSON();
  const char *getAdjJSON();
  const char *getNodeListJSON();
  const char *getRootNodeJSON();

  void traverse();
  static GraphScene *scene_ptr;
};

#ifdef PLATFORM_WEB

extern "C" {
GraphScene *EMSCRIPTEN_KEEPALIVE get_scene_ptr();
void EMSCRIPTEN_KEEPALIVE reset_scene();
void EMSCRIPTEN_KEEPALIVE toggle_keybind_overlay();
void EMSCRIPTEN_KEEPALIVE update_mode(GraphScene *, int, int);
void EMSCRIPTEN_KEEPALIVE on_resize();
void EMSCRIPTEN_KEEPALIVE set_root_node(u_int32_t);
void EMSCRIPTEN_KEEPALIVE set_node_val(u_int32_t, int);
void EMSCRIPTEN_KEEPALIVE set_hover_state(bool, u_int32_t);
void EMSCRIPTEN_KEEPALIVE save_camera_pos();
void EMSCRIPTEN_KEEPALIVE set_camera_pos_to_old_pos();

const char *EMSCRIPTEN_KEEPALIVE get_stack_json();
const char *EMSCRIPTEN_KEEPALIVE get_adj_json();
const char *EMSCRIPTEN_KEEPALIVE get_node_list_json();
const char *EMSCRIPTEN_KEEPALIVE get_root_node_json();

int get_current_algorithm_id();
void start_algo(GraphScene *);
void step_algo(GraphScene *);
}

#endif // PLATFORM_WEB
