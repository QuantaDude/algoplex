#include "events.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "scene.hpp"
#include "state.hpp"
#include "web.hpp"
#include <cstdio>
#include <format>
#include <iterator>
#include <sys/types.h>
#if defined(PLATFORM_WEB)
#include <emscripten/html5.h>

#endif

#ifndef PLATFORM_WEB

void set_mode(int a, int b) {}
#endif // !PLATFORM_WEB

Mat4 Mat4Identity(void) {
  Mat4 m = {0};

  m.m[0] = 1.0f;
  m.m[5] = 1.0f;
  m.m[10] = 1.0f;
  m.m[15] = 1.0f;

  return m;
}
Mat4 Mat4Translate(float x, float y, float z) {
  Mat4 m = Mat4Identity();

  m.m[12] = x;
  m.m[13] = y;
  m.m[14] = z;

  return m;
}

Mat4 Mat4RotateX(float radians) {
  Mat4 m = Mat4Identity();

  float c = cosf(radians);
  float s = sinf(radians);

  m.m[5] = c;
  m.m[6] = s;
  m.m[9] = -s;
  m.m[10] = c;

  return m;
}
Mat4 Mat4Multiply(Mat4 a, Mat4 b) {
  Mat4 r = {0};

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      for (int k = 0; k < 4; k++) {
        r.m[col + row * 4] += a.m[k + row * 4] * b.m[col + k * 4];
      }
    }
  }

  return r;
}

void start_algo(AV::Scene *scene) {
  scene->algorithm_state = AV::Running;
  while (!scene->dfs_stack.empty())
    scene->dfs_stack.pop();

  scene->visited.assign(scene->nodes.size(), false);

  if (!scene->nodes.empty()) {
    scene->dfs_stack.push({scene->root_id, AV::Scene::DFSPhase::ENTER}); // root

    EventDescriptor e(EventAction::AlgoStateUpdate, EventTarget::Stack);
    dispatchSceneEvent(e);
  }
}
void step_algo(AV::Scene *scene) {
  scene->algorithm_state = AV::Stepping;
  scene->traverse();

  EventDescriptor e(EventAction::AlgoStateUpdate, EventTarget::Stack);
  dispatchSceneEvent(e);
}

void update_mode(AV::Scene *ptr, int main, int sub) {

  ptr->main_mode = main;
  ptr->sub_mode = sub;
}

AV::Scene *get_scene_ptr() { return AV::scene_ptr; }

void on_resize(void) { AV::scene_ptr->update_res = true; }
const char *get_stack_json() { return AV::scene_ptr->getStackJSON(); }
const char *get_adj_json() { return AV::scene_ptr->getAdjJSON(); }
const char *get_node_list_json() { return AV::scene_ptr->getNodeListJSON(); }
const char *get_root_node_json() { return AV::scene_ptr->getRootNodeJSON(); }

void reset_scene() { return AV::scene_ptr->resetScene(); }
void toggle_keybind_overlay() { AV::scene_ptr->ToggleKeybindOverlay(); }
void set_root_node(u_int32_t idx) { AV::scene_ptr->setRootNode(idx); }
void set_node_val(u_int32_t node_id, int value) {
  AV::scene_ptr->setNodeVal(node_id, value);
}
void save_camera_pos() {
  AV::scene_ptr->camera_old_offset = AV::scene_ptr->g_camera.offset;

  AV::scene_ptr->camera_old_target = AV::scene_ptr->g_camera.target;
  AV::scene_ptr->camera_old_zoom = AV::scene_ptr->g_camera.zoom;
}
void set_camera_pos_to_old_pos() {
  AV::scene_ptr->g_camera.offset = AV::scene_ptr->camera_old_offset;
  AV::scene_ptr->g_camera.target = AV::scene_ptr->camera_old_target;
  AV::scene_ptr->g_camera.zoom = AV::scene_ptr->camera_old_zoom;

  AV::scene_ptr->hoveredNodeIdx = SIZE_MAX;
  //gotoPos should check if the camera is already at the old pos and then set moveCamera to false in it;
}

void set_hover_state(bool hover, u_int32_t node_id) {
  if (AV::scene_ptr->algorithm_state == AV::Running ||
      AV::scene_ptr->algorithm_state == AV::Stepping) {
    return;
  }

  //if hover is being set as false, go to the old pos.

  if (!hover) {

    AV::scene_ptr->hoveredNodeIdx = SIZE_MAX;
    //gotoPos will run in this condition and move the camera back to old pos;

  } else {

    AV::scene_ptr->hoveredNodeIdx = AV::scene_ptr->id_to_node_idx[node_id];
  }

  AV::scene_ptr->moveCamera = true;
}
int get_current_algorithm_id() { return AV::scene_ptr->getCurrentAlgoId(); }

AV::Scene::Scene(Font *font)

    : a_id(AlgorithmId::DFS_A), algorithm_state(AV::Idle), g_camera({{0}}),
      m_font(font), m_input_mode(InteractionMode::None) {}

namespace AV {
Scene *scene_ptr = nullptr;
}
void AV::Scene::init() {

  AV::scene_ptr = this;
  lastKey = "";

  IVector2 *resolution = App::getInstance().getResolution();

  g_camera.target = {0, 0};
  g_camera.offset = {resolution->x * 0.5f, resolution->y * 0.5f};
  g_camera.zoom = 1.5f;

  Node newNode;
  newNode.id = 0;

  newNode.radius = 15;

  newNode.pos = {g_camera.target.x, g_camera.target.y};
  // newNode.pos = {(float)(resolution->x / 2), (float)(resolution->y / 2)};
  newNode.collider = {newNode.pos.x - newNode.radius,
                      newNode.pos.y - newNode.radius, (float)newNode.radius * 2,
                      (float)newNode.radius * 2};
  newNode.data = 0;

  nodes.push_back(newNode);
  // NOTE: I need to create an event queue.
  // dispatchSceneEvent({EventAction::Add, EventTarget::Node, newNode.id});

  selected_node = nodes.end();
  selected_edge_origin = nodes.end();
  root_id = nodes[0].id;

  // SetMouseScale(resolution->x / 300.0f, resolution->y / 150.0f);
}
void AV::Scene::draw(IVector2 *resolution) {
  if (moveCamera && hoveredNodeIdx != SIZE_MAX) {
    gotoNode(resolution);
  } else if (moveCamera && hoveredNodeIdx == SIZE_MAX) {
    gotoPos(resolution);
  }
  BeginDrawing();
  ClearBackground({0, 0, 0, 0});

  BeginMode2D(g_camera);
  rlPushMatrix();
  rlTranslatef(0, 50 * 50, 0);
  rlRotatef(90, 1, 0, 0);
  DrawGrid(1000, 50);
  rlPopMatrix();

  for (size_t i = 0; i < edges.size(); i++) {
    Color col = (i == hoveredEdgeIdx) ? COLOR_PATH : COLOR_EDGE;
    DrawLineEx(nodes[id_to_node_idx[edges[i].from]].pos,
               nodes[id_to_node_idx[edges[i].to]].pos, 3, col);
  }
  if (selected_edge_origin != nodes.end()) {
    DrawLineEx(selected_edge_origin->pos,
               GetScreenToWorld2D(GetMousePosition(), g_camera), 3, COLOR_PATH);
  }

  for (size_t i = 0; i < nodes.size(); i++) {
    DrawCircleV(nodes[i].pos, nodes[i].radius,
                hoveredNodeIdx == nodes[i].id          ? COLOR_VISITED
                : (selected_node == nodes.begin() + i) ? NORD10
                : (root_id == nodes[i].id)             ? NORD13
                                                       : COLOR_NODE);

    char dataText[10];
    sprintf(dataText, "%d", (int)nodes[i].data);
    Vector2 textSize = MeasureTextEx(*m_font, dataText, 20, 1);
    // DrawText(idText, nodes[i].pos.x - textSize.x / 2,
    //        nodes[i].pos.y - textSize.y / 2, 20, WHITE);
    DrawTextEx(*m_font, dataText, nodes[i].pos - (textSize / 2), 20.0f, 1.0f,
               WHITE);
  }

  EndMode2D();
  AV::Scene::drawUI(*resolution);

  EndDrawing();

  update_input_mode();

#if defined(PLATFORM_WEB)

#elif defined(PLATFORM_DESKTOP)
  resolution->x = GetScreenWidth();
  resolution->y = GetScreenHeight();
#endif
}

void AV::Scene::drawUI(IVector2 resolution) {
  SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  Vector2 mousePos = GetMousePosition();
  char posStr[64];
  char modeStr[32];
  if (show_key_overlay) {

    char keybindStr[256];

    switch (m_input_mode) {
    case InteractionMode::None:
      snprintf(modeStr, sizeof(modeStr), "Free Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nN - Node Mode.\n\nE - "
               "Edge Mode.\n\nLMB - Select Node.\n\nRMB - Select Edge.");
      break;
    case InteractionMode::NodeCreate:
      snprintf(modeStr, std::size(modeStr), "Node Create Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nS - Node Select Mode.\n\nE - "
               "Node Edit "
               "Mode.\n\nLMB - Place New Node.\n\nEsc - Free Mode.");
      break;
    case InteractionMode::NodeEdit:
      snprintf(modeStr, std::size(modeStr), "Node Edit Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nC - Node Create Mode.\n\nS - "
               "Node Select "
               "Mode.\n\nLMB - Select Node.\n\nEsc - Free Mode.");
      break;
    case InteractionMode::NodeSelect:
      snprintf(modeStr, std::size(modeStr), "Node Select Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nC - Node Create Mode.\n\nE - "
               "Node Edit Mode.\n\nLMB - Select Node.\n\nEsc - Free Mode.");

      break;
    case InteractionMode::EdgeCreate:
      snprintf(modeStr, std::size(modeStr), "Edge Create Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nN - Node Select Mode.\n\nE - "
               "Edge Edit"
               "Mode.\n\nC - Edge Create Mode.\n\nRMB - Create "
               "Edge.\n\nEsc - Free Mode.");
      break;
    case InteractionMode::EdgeSelect:
      snprintf(modeStr, std::size(modeStr), "Edge Select Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nN - Node Select Mode.\n\nE - "
               "Edge Edit "
               "Mode.\n\nC - Edge Create Mode.\n\nRMB - Select Edge.\n\nEsc - "
               "Free Mode.");
      break;
    case InteractionMode::EdgeEdit:
      snprintf(modeStr, std::size(modeStr), "Edge Edit Mode");
      snprintf(keybindStr, sizeof(keybindStr),
               "A - Create new node.\n\nN - Node Select Mode.\n\nS - "
               "Edge Select "
               "Mode.\n\nC - Edge Create Mode.\n\nRMB - Select Edge.\n\nEsc - "
               "Free Mode.");
      break;

    default:

      break;
    }

    int keybindStrWidth = MeasureText(keybindStr, 25);
    int modeStrWidth = MeasureText(modeStr, 25);

    DrawText(keybindStr, 50 + (resolution.x - keybindStrWidth) / 2, 100, 25,
             COLOR_TEXT);
    DrawText(modeStr, 50 + (resolution.x - modeStrWidth) / 2, 20, 25,
             COLOR_TEXT);
  }

  snprintf(posStr, sizeof(posStr), "(%.0f, %.0f)", mousePos.x, mousePos.y);
  DrawText(posStr, 10, resolution.y - 30, 25, COLOR_TEXT);

  DrawText(getKeyName(), resolution.x - MeasureText(getKeyName(), 25) - 10,
           resolution.y - 30, 25, COLOR_TEXT);
}
void AV::Scene::update_input_mode() {
  if (main_mode == 0) {
    m_input_mode = InteractionMode::None;
  } else if (main_mode == 1) {
    switch (sub_mode) {
    case 0:
      m_input_mode = InteractionMode::NodeSelect;
      break;
    case 1:
      m_input_mode = InteractionMode::NodeCreate;
      break;
    case 2:
      m_input_mode = InteractionMode::NodeEdit;
      break;
    }
  } else if (main_mode == 2) { // EDGE
    switch (sub_mode) {
    case 0:
      m_input_mode = InteractionMode::EdgeSelect;
      break;
    case 1:
      m_input_mode = InteractionMode::EdgeCreate;
      break;
    case 2:
      m_input_mode = InteractionMode::EdgeEdit;
      break;
    }
  }
}

void AV::Scene::input() {

  mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), g_camera);

  switch (m_input_mode) {

  case InteractionMode::NodeCreate:

    if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool clickedOnNode = false;

        for (size_t i = 0; i < nodes.size(); i++) {
          if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

            clickedOnNode = true;

            if (selected_node == nodes.end()) {
              selected_node = nodes.begin() + i;
              selected_node->oldPos = selected_node->pos;
              break;
            } else if (selected_node != nodes.begin() + i) {
              break;

            } else if (selected_node == nodes.begin() + i) {
              // Deselect if clicking on the same node
              clickedOnNode = false;
            }
          }
        }

        // Deselect if clicking on empty space
        if (!clickedOnNode && selected_node != nodes.end()) {
          selected_node = nodes.end();
          break;
        }

        // create new node
        if (selected_node == nodes.end() &&
            selected_edge_origin == nodes.end()) {
          Node newNode;
          if (nodes.begin() != nodes.end()) {
            newNode.id = (nodes.end() - 1)->id + 1;
          } else {
            newNode.id = 0;
          }
          Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), g_camera);
          newNode.pos = mouseWorld;
          // newNode.oldPos = mouseWorld;
          newNode.radius = 15;
          newNode.collider = {
              newNode.pos.x - newNode.radius, newNode.pos.y - newNode.radius,
              (float)newNode.radius * 2, (float)newNode.radius * 2};
          newNode.data = nodes.size();

          nodes.push_back(newNode);

          id_to_node_idx[newNode.id] = nodes.size() - 1;
          // selectedNode = &nodes.back();
          selected_node = nodes.end();
          selected_edge_origin = nodes.end();

          dispatchSceneEvent(
              {EventAction::Add, EventTarget::Node, nodes.size() - 1});
        }
      }
    }
    break;
  case InteractionMode::NodeSelect:

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      bool clickedOnNode = false;

      for (size_t i = 0; i < nodes.size(); i++) {
        if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

          clickedOnNode = true;

          if (selected_node == nodes.end()) {
            selected_node = nodes.begin() + i;
            selected_node->oldPos = selected_node->pos;
            break;
          } else if (selected_node != nodes.begin() + i) {
            break;

          } else if (selected_node == nodes.begin() + i) {
            // Deselect if clicking on the same node
            clickedOnNode = false;
          }
        }
      }

      // Deselect if clicking on empty space
      if (!clickedOnNode && selected_node != nodes.end()) {
        //        selectedNode = nullptr;
        selected_node = nodes.end();
      }
    }

    if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
      if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_DELETE)) {
        if (selected_node != nodes.end() && nodes.begin() != nodes.end()) {

          for (std::vector<Edge>::iterator edge = edges.begin();
               edge < edges.end();) {

            if (edge->from == selected_node->id ||
                edge->to == selected_node->id) {

              std::erase(nodes[id_to_node_idx[edge->from]].edges, edge->to);
              std::erase(nodes[id_to_node_idx[edge->to]].edges, edge->from);
              std::erase(selected_node->edges, edge->from);
              std::erase(selected_node->edges, edge->to);

              edges.erase(edge);
            } else {
              ++edge;
            }
          }

          nodes.erase(selected_node);
          id_to_node_idx.clear();

          for (size_t i = 0; i < nodes.size(); i++) {
            id_to_node_idx[nodes[i].id] = i;
          }
          selected_node = nodes.end();
          selected_edge_origin = nodes.end();
          dispatchSceneEvent(
              {EventAction::Remove, EventTarget::Node,
               static_cast<u_int32_t>(
                   std::distance(nodes.begin(), selected_node) - 1)});
        }
      }
    }
    break;
  case InteractionMode::NodeEdit:
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      bool clickedOnNode = false;

      for (size_t i = 0; i < nodes.size(); i++) {
        if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

          clickedOnNode = true;
          const Vector2 nodepos = GetWorldToScreen2D(nodes[i].pos, g_camera);
          dispatchUIEvent({EventAction::Edit, EventTarget::Node, nodes[i].id},
                          nodepos, nodes[i].data);
          break;
        }
      }
    }
    break;

  case InteractionMode::EdgeCreate:

    if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {

        for (size_t i = 0; i < nodes.size(); i++) {
          if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

            if (selected_edge_origin == nodes.end()) {
              m_input_mode = InteractionMode::EdgeCreate;
              selected_edge_origin = nodes.begin() + i;
            } else if (selected_edge_origin != nodes.end() &&
                       selected_edge_origin != nodes.begin() + i) {

              Edge newEdge;
              newEdge.from = (selected_edge_origin)->id;
              newEdge.to = nodes[i].id;

              bool edgeExists = false;
              for (const auto &edge : edges) {
                if ((edge.from == newEdge.from && edge.to == newEdge.to) ||
                    (edge.from == newEdge.to && edge.to == newEdge.from)) {
                  edgeExists = true;
                  break;
                }
              }
              if (!edgeExists) {
                edges.push_back(newEdge);

                // Also add to node's edge list
                selected_edge_origin->edges.push_back(newEdge.to);
                // nodes[newEdge.from].edges.push_back(newEdge.to);
                // nodes[newEdge.to].edges.push_back(newEdge.from);
                nodes[i].edges.push_back(newEdge.from);

                dispatchSceneEvent(
                    {EventAction::Add, EventTarget::Edge, newEdge.from});
              }

              selected_edge_origin = nodes.end();
            } else {
              selected_edge_origin = nodes.end();
            }
            return;
          }
        }
        selected_edge_origin = nodes.end();
      }
    }
    break;

  case InteractionMode::EdgeEdit:
    // Maybe highlight edge, allow delete

    if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
      for (size_t i = 0; i < edges.size(); i++) {
        if (IsMouseHoveringEdge(mouse_world_pos, nodes[edges[i].from].pos,
                                nodes[edges[i].to].pos)) {
          m_input_mode = InteractionMode::EdgeEdit;
          hoveredEdgeIdx = i;
          if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
              selected_edge_origin == nodes.end()) {
            selected_edge_origin = nodes.begin() + (edges.begin() + i)->from;
            std::vector<Edge>::iterator it = edges.begin();
            it += i;
            edges.erase(it);
            hoveredEdgeIdx = SIZE_MAX;

            return;
          }
        }
      }
      if (selected_edge_origin != nodes.end() &&
          IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        for (size_t i = 0; i < nodes.size(); i++) {
          if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

            if (selected_edge_origin != nodes.begin() + i) {
              Edge newEdge;
              int originNodeIdx = (int)(selected_edge_origin - nodes.begin());
              newEdge.from = originNodeIdx;
              newEdge.to = i;
              edges.push_back(newEdge);

              nodes[newEdge.from].edges.push_back(newEdge.to);
              nodes[newEdge.to].edges.push_back(newEdge.from);

              dispatchSceneEvent({EventAction::Add, EventTarget::Node,
                                  static_cast<u_int32_t>(std::distance(
                                      nodes.begin(), selected_edge_origin))});

              selected_edge_origin = nodes.end();
            }
          }
        }
      }
    }
    break;

  case InteractionMode::EdgeSelect:

    if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
      for (size_t i = 0; i < edges.size(); i++) {
        if (IsMouseHoveringEdge(mouse_world_pos, nodes[edges[i].from].pos,
                                nodes[edges[i].to].pos)) {
          hoveredEdgeIdx = i;
          if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
              selected_edge_origin == nodes.end()) {
            m_input_mode = InteractionMode::EdgeEdit;
            main_mode = 2;
            sub_mode = 2;

            set_mode(main_mode, sub_mode);
            selected_edge_origin = nodes.begin() + (edges.begin() + i)->from;
            std::vector<Edge>::iterator it = edges.begin();
            it += i;
            edges.erase(it);
            hoveredEdgeIdx = SIZE_MAX;
          }

          return;
        }
      }
    }
    break;
  case InteractionMode::None:
  default:
    // Camera panning, idle state

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (size_t i = 0; i < nodes.size(); i++) {
        if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {

          nodes[i].oldPos = nodes[i].pos;
          selected_node = nodes.begin() + i;
          m_input_mode = InteractionMode::NodeSelect;
          main_mode = 1;
          sub_mode = 0;

          set_mode(main_mode, sub_mode);
          return;
        }
      }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {

      for (size_t i = 0; i < nodes.size(); i++) {
        if (CheckCollisionPointRec(mouse_world_pos, nodes[i].collider)) {
          m_input_mode = InteractionMode::EdgeCreate;
          main_mode = 2;
          sub_mode = 1;

          set_mode(main_mode, sub_mode);
          selected_edge_origin = nodes.begin() + i;
          return;
        }
      }
    }
    for (size_t i = 0; i < edges.size(); i++) {
      if (IsMouseHoveringEdge(mouse_world_pos, nodes[edges[i].from].pos,
                              nodes[edges[i].to].pos)) {
        hoveredEdgeIdx = i;
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
            selected_edge_origin == nodes.end()) {
          m_input_mode = InteractionMode::EdgeEdit;
          main_mode = 2;
          sub_mode = 2;

          set_mode(main_mode, sub_mode);
          selected_edge_origin = nodes.begin() + (edges.begin() + i)->from;
          std::vector<Edge>::iterator it = edges.begin();
          it += i;
          edges.erase(it);
          hoveredEdgeIdx = SIZE_MAX;
        }

        return;
      }
    }

    break;
  }

  // camera controls
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    moveCamera = false;

    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f / g_camera.zoom);
    g_camera.target = Vector2Add(g_camera.target, delta);
  }

  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    moveCamera = false;
    // Get the world point that is under the mouse

    g_camera.offset = GetMousePosition();

    g_camera.target = mouse_world_pos;

    float scale = 0.2f * wheel;

    g_camera.zoom = Clamp(expf(logf(g_camera.zoom) + scale), 0.125f, 64.0f);
  }

  // reset
  if (IsKeyPressed(KEY_ESCAPE)) {
    m_input_mode = InteractionMode::None;
    main_mode = 0;
    // sub_mode = 0;

    set_mode(main_mode, sub_mode);
    if (selected_node != nodes.end()) {
      selected_node->pos = selected_node->oldPos;
      selected_node->collider = {selected_node->pos.x - selected_node->radius,
                                 selected_node->pos.y - selected_node->radius,
                                 (float)selected_node->radius * 2,
                                 (float)selected_node->radius * 2};
    }

    selected_node = nodes.end();
    selected_edge_origin = nodes.end();
  }
  if (algorithm_state == AV::Idle || algorithm_state == AV::Done) {
    if (IsKeyPressed(KEY_A)) {
      m_input_mode = InteractionMode::NodeCreate;
      main_mode = 1;
      sub_mode = 1;
      set_mode(main_mode, sub_mode);
      if (selected_node == nodes.end() && selected_edge_origin == nodes.end()) {
        Node newNode;
        if (nodes.begin() != nodes.end()) {
          newNode.id = (nodes.end() - 1)->id + 1;
        } else {
          newNode.id = 0;
        }

        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), g_camera);
        newNode.pos = mouseWorld;
        newNode.oldPos = mouseWorld;
        newNode.radius = 15;
        newNode.collider = {
            newNode.pos.x - newNode.radius, newNode.pos.y - newNode.radius,
            (float)newNode.radius * 2, (float)newNode.radius * 2};
        newNode.data = nodes.size();

        nodes.push_back(newNode);

        id_to_node_idx[newNode.id] = nodes.size() - 1;
        root_id = nodes[0].id;
        selected_node = nodes.end() - 1;
        selected_edge_origin = nodes.end();

        dispatchSceneEvent(
            {EventAction::Add, EventTarget::Node, nodes.size() - 1});
      }
    }

    if (selected_edge_origin == nodes.end() && selected_node == nodes.end()) {
      if (IsKeyPressed(KEY_N)) {

        // m_input_mode = InteractionMode::NodeSelect;

        main_mode = 1;
        // sub_mode = 0;

        set_mode(main_mode, sub_mode);

      } else if (IsKeyReleased(KEY_E)) {
        if (main_mode == 0) {
          // m_input_mode = InteractionMode::EdgeSelect;

          main_mode = 2;
          // sub_mode = 0;
          set_mode(main_mode, sub_mode);
        } else if (main_mode == 1) {
          sub_mode = 2;
          m_input_mode = InteractionMode::NodeEdit;

          set_mode(main_mode, sub_mode);
        } else if (main_mode == 2) {
          sub_mode = 2;
          m_input_mode = InteractionMode::EdgeEdit;

          set_mode(main_mode, sub_mode);
        }
      }
      if (IsKeyPressed(KEY_S)) {
        sub_mode = 0;

        set_mode(main_mode, sub_mode);
        if (main_mode == 1) {
          m_input_mode = InteractionMode::NodeSelect;

          set_mode(main_mode, sub_mode);
        } else if (main_mode == 2) {
          m_input_mode = InteractionMode::EdgeSelect;

          set_mode(main_mode, sub_mode);
        }
      }
      if (IsKeyPressed(KEY_C)) {
        sub_mode = 1;
        set_mode(main_mode, sub_mode);
      }
    }
  }
}

bool AV::Scene::IsMouseHoveringEdge(const Vector2 &mouse, const Vector2 &p1,
                                    const Vector2 &p2, float thickness) {
  // Vector from p1 to p2
  Vector2 edge = Vector2Subtract(p2, p1);
  Vector2 mouseVec = Vector2Subtract(mouse, p1);

  float edgeLenSq = Vector2LengthSqr(edge);
  if (edgeLenSq == 0.0f)
    return false;

  // Project mouseVec onto edge (clamped to [0,1])
  float t = Vector2DotProduct(mouseVec, edge) / edgeLenSq;
  t = Clamp(t, 0.0f, 1.0f);

  // Closest point on the edge
  Vector2 closest = Vector2Add(p1, Vector2Scale(edge, t));

  // Distance from mouse to closest point
  float dist = Vector2Distance(mouse, closest);
  return dist <= thickness;
}

void AV::Scene::update(IVector2 *resolution) {
  if (update_res) {

    // What world point is currently at the center of the OLD canvas?
    Vector2 old_center_world = GetScreenToWorld2D(
        {resolution->x * 0.5f, resolution->y * 0.5f}, g_camera);

    double css_w, css_h;
    emscripten_get_element_css_size("#canvas", &css_w, &css_h);
    double dpr = emscripten_get_device_pixel_ratio();
    *resolution = {static_cast<int>(std::ceil(css_w * dpr)),
                   static_cast<int>(std::ceil(css_h * dpr))};
    SetWindowSize(resolution->x, resolution->y);

    g_camera.offset = {resolution->x * 0.5f, resolution->y * 0.5f};
    g_camera.target = old_center_world; // keep that same world point centered

    update_res = false;
  } // ic> (resetCamOffset) {
  //
  //   resetCamOffset = false;
  // }
  if (selected_node != nodes.end()) {
    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), g_camera);
    selected_node->pos = mouseWorld;
    selected_node->collider.x =
        mouseWorld.x - selected_node->collider.width / 2;
    selected_node->collider.y =
        mouseWorld.y - selected_node->collider.height / 2;
  }
}

void AV::Scene::traverse() {
  if (dfs_stack.empty()) {
    algorithm_state = AV::Done;
    return;
  }

  DFSFrame frame = dfs_stack.top();
  dfs_stack.pop();

  u_int32_t current = frame.node;

  if (frame.phase == DFSPhase::ENTER) {
    if (visited[current])
      return;
    // we should only update the stack if it is not visited
    //

    visited[current] = true;
    hoveredNodeIdx = current;

    dfs_stack.push({current, DFSPhase::EXIT});

    // Push children in reverse order
    for (int i = nodes[id_to_node_idx[current]].edges.size() - 1; i >= 0; --i) {
      u_int32_t neighbor = nodes[id_to_node_idx[current]].edges[i];
      if (!visited[neighbor]) {
        dfs_stack.push({neighbor, DFSPhase::ENTER});
      }
    }
  } else {
    hoveredNodeIdx = current;
  }
}

const char *AV::Scene::getStackJSON() {

  static std::string result;

  std::stack<DFSFrame> copy = dfs_stack;
  result.clear();

  auto out = std::back_inserter(result);
  std::format_to(out, "[");

  bool first = true;
  while (!copy.empty()) {
    const DFSFrame &f = copy.top();
    if (!first)
      std::format_to(out, ",");
    std::format_to(out, GET_STACK_FMT(DFS_A), f.node,
                   dfsPhaseToString(f.phase));

    copy.pop();
    first = false;
  }

  std::format_to(out, "]");
  return result.c_str();
}

const char *AV::Scene::getAdjJSON() {
  static std::string result;
  result.clear();

  auto out = std::back_inserter(result);
  std::format_to(out, "[");

  bool firstNode = true;
  for (const auto &node : nodes) {
    if (!firstNode)
      std::format_to(out, ",");

    // build edges string first
    std::string edges;
    auto edgeOut = std::back_inserter(edges);
    bool firstEdge = true;
    for (int edge : node.edges) {
      if (!firstEdge)
        std::format_to(edgeOut, ",");
      std::format_to(edgeOut, "{}", edge);
      firstEdge = false;
    }

    // now all variables are ready — single format_to call per node
    std::format_to(out, GET_ADJ_MAT_FMT(DFS_A), node.id, edges);

    firstNode = false;
  }

  std::format_to(out, "]");
  return result.c_str();
}

const char *AV::Scene::getNodeListJSON() {
  static std::string result;
  result.clear();

  auto out = std::back_inserter(result);
  std::format_to(out, "[");

  bool firstNode = true;
  for (const auto &node : nodes) {
    if (!firstNode)
      std::format_to(out, ",");

    std::format_to(out, GRAPH_NODE_LIST_FMT, node.id, node.data);

    firstNode = false;
  }

  std::format_to(out, "]");
  return result.c_str();
}

const char *AV::Scene::getRootNodeJSON() {

  static std::string result;
  result.clear();

  auto out = std::back_inserter(result);

  std::format_to(out, GRAPH_NODE_FMT, root_id,
                 nodes[id_to_node_idx[root_id]].data);

  return result.c_str();
}

void AV::Scene::setRootNode(u_int32_t idx) {
  root_id = idx;
  dispatchSceneEvent({EventAction::Edit, EventTarget::Node, root_id});
}

void AV::Scene::setNodeVal(u_int32_t node_id, int value) {
  nodes[id_to_node_idx[node_id]].data = value;
  dispatchSceneEvent({EventAction::Edit, EventTarget::Node, node_id});
}

void AV::Scene::resetScene() {
  nodes.clear();
  edges.clear();
  id_to_node_idx.clear();
  visited.clear();
  // dfs_stack.clear();
  selected_node = nodes.end();
  hoveredNodeIdx = SIZE_MAX;
  hoveredEdgeIdx = SIZE_MAX;
  selected_edge_origin = nodes.end();
  root_id = 0;
  while (!dfs_stack.empty()) {
    dfs_stack.pop();
  }
  dispatchSceneEvent({EventAction::Remove, EventTarget::Node, 0});
  dispatchSceneEvent({EventAction::Remove, EventTarget::Edge, 0});
  dispatchSceneEvent({EventAction::AlgoStateUpdate, EventTarget::Stack, 0});
}

static bool NearlyEqual(float a, float b, float eps = 0.01f) {
  return fabsf(a - b) <= eps;
}

static bool Vector2NearlyEqual(Vector2 a, Vector2 b, float eps = 0.01f) {
  return NearlyEqual(a.x, b.x, eps) && NearlyEqual(a.y, b.y, eps);
}
void AV::Scene::gotoNode(IVector2 *resolution) {
  Vector2 node_pos = nodes[id_to_node_idx[hoveredNodeIdx]].pos;

  g_camera.target = Vector2Lerp(g_camera.target, node_pos, 0.1f);
  g_camera.zoom = Lerp(g_camera.zoom, 2.0f, 0.05f);
  g_camera.offset = Vector2Lerp(
      g_camera.offset,
      {(float)resolution->x / 2.0f, (float)resolution->y / 2.0f}, 0.1f);
  if (Vector2NearlyEqual(g_camera.target, node_pos) &&
      NearlyEqual(g_camera.zoom, 2.0f) &&
      Vector2NearlyEqual(g_camera.offset, {(float)resolution->x / 2.0f,
                                           (float)resolution->y / 2.0f})) {
    moveCamera = false;
  }
}
void AV::Scene::gotoPos(IVector2 *resolution) {
  g_camera.target = Vector2Lerp(g_camera.target, camera_old_target, 0.1f);
  g_camera.zoom = Lerp(g_camera.zoom, camera_old_zoom, 0.05f);
  g_camera.offset = Vector2Lerp(g_camera.offset, camera_old_offset, 0.1f);

  if (Vector2NearlyEqual(g_camera.target, camera_old_target) &&
      NearlyEqual(g_camera.zoom, camera_old_zoom) &&
      Vector2NearlyEqual(g_camera.offset, camera_old_offset)) {
    moveCamera = false;
  }
}
void AV::Scene::ToggleKeybindOverlay() { show_key_overlay = !show_key_overlay; }

int AV::Scene::getCurrentAlgoId() { return std::to_underlying(a_id); }

