//
//  ruvi base
//

#pragma once

// includes
#include "../../core/interfaces/interfaces.hh"
#include "../render/render.hh"
#include "../structs/entities.hh"
#include "../vector/vector.hh"
#include "math.hh"
#include <cmath>

namespace utilities {

struct esp_box_t {
  int x, y, z, width, height;
};

inline void move_fix(c_user_cmd *cmd, qangle old_angle, float old_forward_move,
                     float old_side_move) {

  float yaw_delta = cmd->view_angles.y - old_angle.y;
  float f1, f2;

  if (old_angle.y < 0.f)
    f1 = 360.f + old_angle.y;
  else
    f1 = old_angle.y;

  if (cmd->view_angles.y < 0.f)
    f2 = 360.f + cmd->view_angles.y;
  else
    f2 = cmd->view_angles.y;

  if (f2 < f1)
    yaw_delta = std::abs(f2 - f1);
  else
    yaw_delta = 360.f - std::abs(f1 - f2);

  yaw_delta = 360.f - yaw_delta;

  cmd->forward_move = std::cos(DEG2RAD(yaw_delta)) * old_forward_move +
                      std::cos(DEG2RAD(yaw_delta + 90.f)) * old_side_move;
  cmd->side_move = std::sin(DEG2RAD(yaw_delta)) * old_forward_move +
                   std::sin(DEG2RAD(yaw_delta + 90.f)) * old_side_move;
}

inline vector3d rotate_point(vector3d point_to_rotate, int center_x,
                             int center_y, float angle) {

  // angle in radians
  float angle_in_radians = angle * (M_PI / 180.f);

  // sin and cos theta
  float sin_theta = std::sin(angle_in_radians);
  float cos_theta = std::cos(angle_in_radians);

  return {(cos_theta * (point_to_rotate.x - center_x) -
           sin_theta * (point_to_rotate.y - center_y) + center_x),
          (sin_theta * (point_to_rotate.x - center_x) +
           cos_theta * (point_to_rotate.y - center_y) + center_y),
          0.f};
}

inline bool create_box(entity_t *entity, esp_box_t &box) {

  // get the entity origin (position)
  vector3d origin = entity->m_vecOrigin();

  // get entity dimensions
  vector3d min = entity->m_vecMins() + origin;
  vector3d max = entity->m_vecMaxs() + origin;

  // points of a 3d bounding box
  vector3d points[] = {
      vector3d(min.x, min.y, min.z), vector3d(min.x, max.y, min.z),
      vector3d(max.x, max.y, min.z), vector3d(max.x, min.y, min.z),
      vector3d(max.x, max.y, max.z), vector3d(min.x, max.y, max.z),
      vector3d(min.x, min.y, max.z), vector3d(max.x, min.y, max.z)};

  // box dimensions
  vector3d front_left_bottom, back_right_top, back_left_bottom, front_right_top,
      front_right_bottom, back_right_bottom, back_left_top, front_left_top;

  // get screen positions
  if (!draw::world_to_screen(points[3], front_left_bottom) ||
      !draw::world_to_screen(points[5], back_right_top) ||
      !draw::world_to_screen(points[0], back_left_bottom) ||
      !draw::world_to_screen(points[4], front_right_top) ||
      !draw::world_to_screen(points[2], front_right_bottom) ||
      !draw::world_to_screen(points[1], back_right_bottom) ||
      !draw::world_to_screen(points[6], back_left_top) ||
      !draw::world_to_screen(points[7], front_left_top)) {
    return false;
  }

  // put everything on a array
  vector3d box_array[] = {
      front_left_bottom,  back_right_top,    back_left_bottom, front_right_top,
      front_right_bottom, back_right_bottom, back_left_top,    front_left_top};

  // initialize
  int left   = front_left_bottom.x;
  int top    = front_left_bottom.y;
  int right  = front_left_bottom.x;
  int bottom = front_left_bottom.y;

  // find the bounding corners for our box
  for (std::size_t i = 1; i < 8; i++) {

    if (left > box_array[i].x) left = box_array[i].x;

    if (bottom < box_array[i].y) bottom = box_array[i].y;

    if (right < box_array[i].x) right = box_array[i].x;

    if (top > box_array[i].y) top = box_array[i].y;
  }

  box.x      = static_cast<int>(left);
  box.y      = static_cast<int>(top);
  box.width  = static_cast<int>((right - left));
  box.height = static_cast<int>((bottom - top));

  return true;
}

inline void create_materials() {

  std::ofstream("csgo//materials//normal_material.vmt")
      << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "0"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "1"
			"$wireframe"    "0"
        })#";

  std::ofstream("csgo//materials//ignorez_material.vmt")
      << R"#("VertexLitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "1"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "1"
			"$wireframe"    "0"
        })#";

  std::ofstream("csgo//materials//glow_material.vmt") << R"#("UnlitGeneric" {
            "$basetexture" "vgui/white_additive"
            "$ignorez"      "1"
            "$envmap"       ""
            "$nofog"        "1"
            "$model"        "1"
            "$nocull"       "0"
            "$selfillum"    "1"
            "$halflambert"  "1"
            "$znearer"      "0"
            "$flat"         "1"
			"$wireframe"    "1"
        })#";

  std::ofstream("csgo//materials//mirrorcam_material.vmt")
      << R"#("UnlitGeneric" {
            "$basetexture" "mirrorcam_texture"
      })#";
}

inline vector3d angle_to_pixels(const vector3d &angle) {

  // m_yaw convar
  static convar *m_yaw = csgo::cvar->find_var(STR("m_yaw"));

  // m_pitch convar
  static convar *m_pitch = csgo::cvar->find_var(STR("m_pitch"));

  float x = angle.x / m_pitch->get_float();
  float y = angle.y / m_yaw->get_float();

  return vector3d(-y, x, 0.f);
}

inline vector3d pixels_to_angle(const vector3d &pixel) {

  // m_yaw convar
  static convar *m_yaw = csgo::cvar->find_var(STR("m_yaw"));

  // m_pitch convar
  static convar *m_pitch = csgo::cvar->find_var(STR("m_pitch"));

  float x = pixel.x * m_pitch->get_float();
  float y = pixel.y * m_yaw->get_float();

  return vector3d(-y, x, 0.f).clamp();
}

inline vector3d get_hitbox_position(entity_t *entity, int hitbox_id) {

  matrix3x4_t bone_matrix[128];

  if (entity->setup_bones(bone_matrix, 128, 0x100, 0.0f)) {

    // studio model pointer
    studiohdr_t *studio_model =
        csgo::model_info->get_studio_model(entity->get_model());

    if (studio_model) {

      // search for the hitbox that we are looking for
      mstudiobbox_t *hitbox = studio_model->hitbox_set(0)->hitbox(hitbox_id);

      // if we have a valid hitbox
      if (hitbox) {

        vector3d min = {0.f, 0.f, 0.f};
        vector3d max = {0.f, 0.f, 0.f};

        math::vector_transform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
        math::vector_transform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

        return (min + max) / 2.0f;
      }
    }
  }

  return vector3d{};
}

} // namespace utilities