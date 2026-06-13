#include "colors.h"
#include "raylib.h"
#include "state.hpp"
#include <cstddef>
#include <cstdint>
#include <stack>
#include <vector>
#if defined(PLATFORM_WEB)
#import <emscripten.h>
#include <emscripten/em_macros.h>
#import <emscripten/em_types.h>
#endif // PLATOFRM_WEB

typedef struct {
  float m[16];
} Mat4;

namespace AV {

class Scene : public State {

public:
  enum class DFSPhase { ENTER, EXIT };

  int VIRTUAL_W= 1920, VIRTUAL_H = 1080;
  RenderTexture2D target;
  struct DFSFrame {
    int node;
    DFSPhase phase;
  };
  AlgorithmState algorithm_state;
  Camera2D g_camera;
  Vector2 mouse_world_pos;
  Font *m_font;

  // void GuiAlgoViz(BaseGuiState *state);

  static void Button006();
  static void StopButton(Scene *);
  static void StartButton(Scene *);
  static void StepButton(Scene *);
  static void Button015();
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
    Vector2 pos;
    Vector2 oldPos;
    uint16_t radius;
    Rectangle collider;

    int64_t data;
    std::vector<int> edges;
  };

  Node *root;
  // allocate array of nodes
  std::vector<Node> nodes;
  std::vector<Edge> edges;
  std::stack<DFSFrame> dfs_stack;
  std::vector<bool> visited;

  std::vector<Node>::iterator selected_node;
  std::vector<Node>::iterator selected_edge_origin;
  size_t hoveredEdgeIdx = SIZE_MAX;
  size_t hoveredNodeIdx = SIZE_MAX;

  Scene(Font *);
  void init() override;
  void input() override;
  void update_input_mode();
  void draw(IVector2 *) override;
  void drawUI(IVector2);
  void update() override;
  bool IsMouseHoveringEdge(const Vector2 &, const Vector2 &, const Vector2 &,
                           float thickness = 5.0f);

  void dfs();
  void createStack();
  void drawDFSStack(Rectangle);
  void traverse();
};

extern AV::Scene *scene_ptr;
}; // namespace AV

#ifdef PLATFORM_WEB

extern "C" {
AV::Scene *EMSCRIPTEN_KEEPALIVE get_scene_ptr();
void EMSCRIPTEN_KEEPALIVE update_mode(AV::Scene *, int, int);
void EMSCRIPTEN_KEEPALIVE on_resize();
void start_algo(AV::Scene *);
void step_algo(AV::Scene *);
}
#endif // PLATFORM_WEB
