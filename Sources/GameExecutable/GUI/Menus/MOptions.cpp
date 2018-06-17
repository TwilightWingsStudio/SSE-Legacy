/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "MenuStarters.h"
#include "MOptions.h"

// --------------------------------------------------------------------------------------
// Intializes options menu.
// --------------------------------------------------------------------------------------
void COptionsMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("OPTIONS");

  // Initialize "Video Options" button.
  gm_mgVideoOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgVideoOptions.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgVideoOptions.mg_pmgUp = &gm_mgAddonOptions;
  gm_mgVideoOptions.mg_pmgDown = &gm_mgAudioOptions;
  gm_mgVideoOptions.mg_strText = TRANS("VIDEO OPTIONS");
  gm_mgVideoOptions.mg_strTip = TRANS("set video mode and driver");

  // Initialize "Audio Options" button.
  gm_mgAudioOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgAudioOptions.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgAudioOptions.mg_pmgUp = &gm_mgVideoOptions;
  gm_mgAudioOptions.mg_pmgDown = &gm_mgPlayerProfileOptions;
  gm_mgAudioOptions.mg_strText = TRANS("AUDIO OPTIONS");
  gm_mgAudioOptions.mg_strTip = TRANS("set audio quality and volume");

  // Initialize "Players And Controls" button.
  gm_mgPlayerProfileOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgPlayerProfileOptions.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgPlayerProfileOptions.mg_pmgUp = &gm_mgAudioOptions;
  gm_mgPlayerProfileOptions.mg_pmgDown = &gm_mgNetworkOptions;
  gm_mgPlayerProfileOptions.mg_strText = TRANS("PLAYERS AND CONTROLS");
  gm_mgPlayerProfileOptions.mg_strTip = TRANS("change currently active player or adjust controls");

  // Initialize "Network Connection" button.
  gm_mgNetworkOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgNetworkOptions.mg_boxOnScreen = BoxBigRow(3);
  gm_mgNetworkOptions.mg_pmgUp = &gm_mgPlayerProfileOptions;
  gm_mgNetworkOptions.mg_pmgDown = &gm_mgCustomOptions;
  gm_mgNetworkOptions.mg_strText = TRANS("NETWORK CONNECTION");
  gm_mgNetworkOptions.mg_strTip = TRANS("choose your connection parameters");

  // Initialize "Advanced Options" button.
  gm_mgCustomOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgCustomOptions.mg_boxOnScreen = BoxBigRow(4);
  gm_mgCustomOptions.mg_pmgUp = &gm_mgNetworkOptions;
  gm_mgCustomOptions.mg_pmgDown = &gm_mgAddonOptions;
  gm_mgCustomOptions.mg_strText = TRANS("ADVANCED OPTIONS");
  gm_mgCustomOptions.mg_strTip = TRANS("for advanced users only");

  // Initialize "Execute Addon" button.
  gm_mgAddonOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgAddonOptions.mg_boxOnScreen = BoxBigRow(5);
  gm_mgAddonOptions.mg_pmgUp = &gm_mgCustomOptions;
  gm_mgAddonOptions.mg_pmgDown = &gm_mgVideoOptions;
  gm_mgAddonOptions.mg_strText = TRANS("EXECUTE ADDON");
  gm_mgAddonOptions.mg_strTip = TRANS("choose from list of addons to execute");
  
  // Add components.
  AddChild(&gm_mgTitle);
  AddChild(&gm_mgVideoOptions);
  AddChild(&gm_mgAudioOptions);
  AddChild(&gm_mgPlayerProfileOptions);
  AddChild(&gm_mgNetworkOptions);
  AddChild(&gm_mgCustomOptions);
  AddChild(&gm_mgAddonOptions);
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL COptionsMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == &gm_mgVideoOptions) {
      StartVideoOptionsMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == &gm_mgAudioOptions) {
      StartAudioOptionsMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == &gm_mgPlayerProfileOptions) {
      StartChangePlayerMenuFromOptions();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgNetworkOptions) {
      StartNetworkSettingsMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgCustomOptions) {
      StartCustomLoadMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgAddonOptions) {
      StartAddonsLoadMenu();
      return TRUE;
      
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}