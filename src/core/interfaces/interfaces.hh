//
//  ruvi base
//

#pragma once

// GLOBAL INCLUDES
#include "../definitions/handler.hh"
#include "../security/stringcrypt.hh"

// includes
#include <array>
#include <dlfcn.h>
#include <sstream>
#include <string>

// interfaces
#include "sdk/valve/cbaseclientstate.hh"
#include "sdk/valve/cclientclass.hh"
#include "sdk/valve/cglobalvars.hh"
#include "sdk/valve/cinput.hh"
#include "sdk/valve/cmodelinfo.hh"
#include "sdk/valve/crecvproxydata.hh"
#include "sdk/valve/crenderview.hh"
#include "sdk/valve/cusercmd.hh"
#include "sdk/valve/ibaseclient.hh"
#include "sdk/valve/icliententitylist.hh"
#include "sdk/valve/iclientmode.hh"
#include "sdk/valve/icvar.hh"
#include "sdk/valve/iengineclient.hh"
#include "sdk/valve/ienginevgui.hh"
#include "sdk/valve/iinputinternal.hh"
#include "sdk/valve/iinputsystem.hh"
#include "sdk/valve/imaterial.hh"
#include "sdk/valve/imaterialsystem.hh"
#include "sdk/valve/imatrendercontext.hh"
#include "sdk/valve/ipanel.hh"
#include "sdk/valve/iprediction.hh"
#include "sdk/valve/isurface.hh"
#include "sdk/valve/ivdebugoverlay.hh"
#include "sdk/valve/ivmodelrender.hh"
#include "sdk/valve/viewshared.hh"
#include "sdk/valve/iviewrender.hh"
#include "sdk/valve/ienginetrace.hh"

typedef void *(*create_interface_fn)();

struct interface_reg_t {

  create_interface_fn create_fn;
  const char *        interface_name;
  interface_reg_t *   next;
};

namespace interfaces {

void on_entry_point();

template <typename t> // this is not finished yet
inline t *get_interface(const std::string &interface_location) {

  std::string_view         dll_path, delims = "->";
  std::vector<std::string> splitted_string;

  // split string (tokenize)
  std::size_t start = interface_location.find_first_not_of(delims), end = 0;
  while ((end = interface_location.find_first_of(delims, start)) !=
         std::string::npos) {

    splitted_string.push_back(interface_location.substr(start, end - start));
    start = interface_location.find_first_not_of(delims, end);
  }

  if (start != std::string::npos)
    splitted_string.push_back(interface_location.substr(start));

  // fetch dll path
  if (splitted_string.at(0).find(STR("client_panorama_client.so")) !=
      std::string::npos)
    dll_path = STR("./csgo/bin/linux64/");
  else
    dll_path = STR("./bin/linux64/");

  // insert dll location
  splitted_string.at(0).insert(0, dll_path);

  // get module handle
  void *module = dlopen(splitted_string.at(0).c_str(),
                        RTLD_NOLOAD | RTLD_NOW | RTLD_LOCAL);

  // grab the interface list
  interface_reg_t *interfaces_sym = *reinterpret_cast<interface_reg_t **>(
      dlsym(module, STR("s_pInterfaceRegs")));

  // iterate through the interface list and grab the correct interface
  for (interface_reg_t *current = interfaces_sym; current != nullptr;
       current                  = current->next) {

    if (std::string(current->interface_name).find(splitted_string.at(1)) !=
        std::string::npos) {

      // found interface
      if (std::optional<t *> interface_found =
              reinterpret_cast<t *>(current->create_fn())) {

        if (interface_found.has_value()) return interface_found.value();
      }
    }
  }

  // remove splitted_string
  splitted_string.clear();

  return {};
}
} // namespace interfaces

namespace csgo {

inline i_surface *           vgui_surface;
inline i_panel *             vgui_panel;
inline i_engine_vgui *       engine_vgui;
inline i_engine_client *     engine_client;
inline i_input_system *      input_system;
inline i_input_internal *    input_internal;
inline iv_debug_overlay *    debug_overlay;
inline i_base_client_dll *   base_client;
inline i_client_entity_list *entity_list;
inline i_client_mode *       client_mode;
inline c_base_client_state * client_state;
inline i_cvar *              cvar;
inline i_material_system *   material_system;
inline c_global_vars *       global_vars;
inline c_input *             input;
inline iv_model_render *     model_render;
inline c_model_info *        model_info;
inline c_render_view *       render_view;
inline i_prediction *        prediction;
inline i_view_render *       view_render;
inline i_engine_trace *      engine_trace;
} // namespace csgo