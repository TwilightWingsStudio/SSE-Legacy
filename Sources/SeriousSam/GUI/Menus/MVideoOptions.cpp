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
#include "MenuStuff.h"
#include "MenuStarters.h"
#include "MVideoOptions.h"

extern void InitVideoOptionsButtons();
extern void UpdateVideoOptionsButtons(INDEX iSelected);

// --------------------------------------------------------------------------------------
// Intializes video options menu.
// --------------------------------------------------------------------------------------
void CVideoOptionsMenu::Initialize_t(void)
{
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("VIDEO");
  AddChild(&gm_mgTitle);

  TRIGGER_MG(gm_mgDisplayAPITrigger, 0,
    gm_mgApply, gm_mgDisplayAdaptersTrigger, TRANS("GRAPHICS API"), astrDisplayAPIRadioTexts);

  TRIGGER_MG(gm_mgDisplayAdaptersTrigger, 1,
    gm_mgDisplayAPITrigger, gm_mgDisplayPrefsTrigger, TRANS("DISPLAY ADAPTER"), astrNoYes);

  TRIGGER_MG(gm_mgDisplayPrefsTrigger, 2,
    gm_mgDisplayAdaptersTrigger, gm_mgResolutionsTrigger, TRANS("PREFERENCES"), astrDisplayPrefsRadioTexts);

  TRIGGER_MG(gm_mgAspectRatioTrigger, 3,
    gm_mgDisplayPrefsTrigger, gm_mgResolutionsTrigger, TRANS("ASPECT RATIO"), astrDisplayAspectRatioTexts);
    
  TRIGGER_MG(gm_mgResolutionsTrigger, 4,
    gm_mgAspectRatioTrigger, gm_mgFullScreenCheckBox, TRANS("RESOLUTION"), astrNoYes);
    
  gm_mgFullScreenCheckBox.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgFullScreenCheckBox.mg_boxOnScreen = BoxMediumRow(5.0f);
  gm_mgFullScreenCheckBox.mg_pmgUp = &gm_mgResolutionsTrigger;
  gm_mgFullScreenCheckBox.mg_pmgDown = &gm_mgBitsPerPixelTrigger;
  gm_mgFullScreenCheckBox.mg_strText = TRANS("FULLSCREEN");
  
  AddChild(&gm_mgFullScreenCheckBox);

  TRIGGER_MG(gm_mgBitsPerPixelTrigger, 6,
    gm_mgFullScreenCheckBox, gm_mgVideoRendering, TRANS("BITS PER PIXEL"), astrBitsPerPixelRadioTexts);
    
  gm_mgDisplayAPITrigger.mg_strTip = TRANS("choose graphics API to be used");
  gm_mgDisplayAdaptersTrigger.mg_strTip = TRANS("choose display adapter to be used");
  gm_mgDisplayPrefsTrigger.mg_strTip = TRANS("balance between speed and rendering quality, depending on your system");
  gm_mgResolutionsTrigger.mg_strTip = TRANS("select video mode resolution");
  gm_mgFullScreenCheckBox.mg_strTip = TRANS("make game run in a window or in full screen");
  gm_mgBitsPerPixelTrigger.mg_strTip = TRANS("select number of colors used for display");

   // Initialize "Rendering Options" button.
  gm_mgVideoRendering.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgVideoRendering.mg_boxOnScreen = BoxMediumRow(8.0f);
  gm_mgVideoRendering.mg_pmgUp = &gm_mgBitsPerPixelTrigger;
  gm_mgVideoRendering.mg_pmgDown = &gm_mgApply;
  gm_mgVideoRendering.mg_strText = TRANS("RENDERING OPTIONS");
  gm_mgVideoRendering.mg_strTip = TRANS("manually adjust rendering settings");

  // Initialize "Apply" button.
  gm_mgApply.mg_bfsFontSize = BFS_LARGE;
  gm_mgApply.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgApply.mg_pmgUp = &gm_mgVideoRendering;
  gm_mgApply.mg_pmgDown = &gm_mgDisplayAPITrigger;
  gm_mgApply.mg_strText = TRANS("APPLY");
  gm_mgApply.mg_strTip = TRANS("apply selected options");

  // Add components.
  AddChild(&gm_mgVideoRendering);
  AddChild(&gm_mgApply);
}

void CVideoOptionsMenu::StartMenu(void)
{
  InitVideoOptionsButtons();

  CGameMenu::StartMenu();

  UpdateVideoOptionsButtons(-1);
}

extern void ApplyVideoOptions(void);

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CVideoOptionsMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_TRIGGERED)
    {
      if (event.GuiEvent.Caller == &gm_mgVideoRendering) {
        StartRenderingOptionsMenu();
        return TRUE;

      } else if (event.GuiEvent.Caller == &gm_mgApply) {
        ApplyVideoOptions();
        return TRUE;
      }
    
    } else if (event.GuiEvent.EventType == EGET_CHANGED) {
    
      if (event.GuiEvent.Caller == &gm_mgFullScreenCheckBox) {
        UpdateVideoOptionsButtons(0);
        return TRUE;
        
      } else if (event.GuiEvent.Caller == &gm_mgDisplayPrefsTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;
        
      } else if (event.GuiEvent.Caller == &gm_mgDisplayAPITrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == &gm_mgDisplayAdaptersTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == &gm_mgAspectRatioTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == &gm_mgResolutionsTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == &gm_mgBitsPerPixelTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;
      }
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}