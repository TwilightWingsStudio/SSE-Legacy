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
  gm_pTitle = new CMGTitle(TRANS("VIDEO"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Graphics API" trigger.
  gm_pDisplayAPITrigger = new CMGTrigger(TRANS("GRAPHICS API"));
  gm_pDisplayAPITrigger->mg_strTip = TRANS("choose graphics API to be used");
  gm_pDisplayAPITrigger->mg_boxOnScreen = BoxMediumRow(0.0F);
  gm_pDisplayAPITrigger->mg_astrTexts = astrDisplayAPIRadioTexts;
  gm_pDisplayAPITrigger->mg_ctTexts = sizeof(astrDisplayAPIRadioTexts) / sizeof(astrDisplayAPIRadioTexts[0]);;

  // Initialize "Display Adapter" trigger.
  gm_pDisplayAdaptersTrigger = new CMGTrigger(TRANS("DISPLAY ADAPTER"));
  gm_pDisplayAdaptersTrigger->mg_strTip = TRANS("choose display adapter to be used");
  gm_pDisplayAdaptersTrigger->mg_boxOnScreen = BoxMediumRow(1.0F);
  gm_pDisplayAdaptersTrigger->mg_astrTexts = astrNoYes;

  // Initialize "Preferences" trigger.
  gm_pDisplayPrefsTrigger = new CMGTrigger(TRANS("PREFERENCES"));
  gm_pDisplayPrefsTrigger->mg_strTip = TRANS("balance between speed and rendering quality, depending on your system");
  gm_pDisplayPrefsTrigger->mg_boxOnScreen = BoxMediumRow(2.0F);
  gm_pDisplayPrefsTrigger->mg_astrTexts = astrDisplayPrefsRadioTexts;
  gm_pDisplayPrefsTrigger->mg_ctTexts = sizeof(astrDisplayPrefsRadioTexts) / sizeof(astrDisplayPrefsRadioTexts[0]);

  // Initialize "Aspect Ratio" trigger.
  gm_pAspectRatioTrigger = new CMGTrigger(TRANS("ASPECT RATIO"));
  gm_pAspectRatioTrigger->mg_boxOnScreen = BoxMediumRow(3.0F);
  gm_pAspectRatioTrigger->mg_astrTexts = astrDisplayAspectRatioTexts;
  gm_pAspectRatioTrigger->mg_ctTexts = sizeof(astrDisplayAspectRatioTexts) / sizeof(astrDisplayAspectRatioTexts[0]);
  gm_pAspectRatioTrigger->mg_iSelected = 0;
  gm_pAspectRatioTrigger->ApplyCurrentSelection();

  // Initialize "Resolution" trigger.
  gm_pResolutionsTrigger = new CMGTrigger(TRANS("RESOLUTION"));
  gm_pResolutionsTrigger->mg_strTip = TRANS("select video mode resolution");
  gm_pResolutionsTrigger->mg_boxOnScreen = BoxMediumRow(4.0F);
  gm_pResolutionsTrigger->mg_astrTexts = astrNoYes;
  gm_pResolutionsTrigger->mg_iSelected = 0;

  // Initialize "Fullscreen" checkbox.
  gm_pFullScreenCheckBox = new CMGCheckBox();    
  gm_pFullScreenCheckBox->mg_strText = TRANS("FULLSCREEN");
  gm_pFullScreenCheckBox->mg_strTip = TRANS("make game run in a window or in full screen");
  gm_pFullScreenCheckBox->mg_bfsFontSize = BFS_MEDIUM;
  gm_pFullScreenCheckBox->mg_boxOnScreen = BoxMediumRow(5.0f);
  gm_pFullScreenCheckBox->mg_pmgUp = gm_pResolutionsTrigger;
  gm_pFullScreenCheckBox->mg_pmgDown = gm_pBitsPerPixelTrigger;
  
  // Initialize "Bits Per Pixel" trigger.
  gm_pBitsPerPixelTrigger = new CMGTrigger(TRANS("BITS PER PIXEL"));
  gm_pBitsPerPixelTrigger->mg_strTip = TRANS("select number of colors used for display");
  gm_pBitsPerPixelTrigger->mg_boxOnScreen = BoxMediumRow(6.0f);
  gm_pBitsPerPixelTrigger->mg_astrTexts = astrBitsPerPixelRadioTexts;
  gm_pBitsPerPixelTrigger->mg_ctTexts = sizeof(astrBitsPerPixelRadioTexts) / sizeof(astrBitsPerPixelRadioTexts[0]);

  // Initialize "Rendering Options" button.
  gm_pVideoRendering = new CMGButton(TRANS("RENDERING OPTIONS"));
  gm_pVideoRendering->mg_strTip = TRANS("manually adjust rendering settings");
  gm_pVideoRendering->mg_bfsFontSize = BFS_MEDIUM;
  gm_pVideoRendering->mg_boxOnScreen = BoxMediumRow(8.0f);

  // Initialize "Apply" button.
  gm_pApply = new CMGButton(TRANS("APPLY"));
  gm_pApply->mg_strTip = TRANS("apply selected options");
  gm_pApply->mg_bfsFontSize = BFS_LARGE;
  gm_pApply->mg_boxOnScreen = BoxBigRow(6.0f);
  
  // Define neighbours.
  gm_pDisplayAPITrigger->mg_pmgUp = gm_pApply;
  gm_pDisplayAPITrigger->mg_pmgDown = gm_pDisplayAdaptersTrigger;
  gm_pDisplayAdaptersTrigger->mg_pmgUp = gm_pDisplayAPITrigger;
  gm_pDisplayAdaptersTrigger->mg_pmgDown = gm_pDisplayPrefsTrigger;
  gm_pDisplayPrefsTrigger->mg_pmgUp = gm_pDisplayAdaptersTrigger;
  gm_pDisplayPrefsTrigger->mg_pmgDown = gm_pAspectRatioTrigger;
  gm_pAspectRatioTrigger->mg_pmgUp = gm_pDisplayPrefsTrigger;
  gm_pAspectRatioTrigger->mg_pmgDown = gm_pResolutionsTrigger;
  gm_pResolutionsTrigger->mg_pmgUp = gm_pAspectRatioTrigger;
  gm_pResolutionsTrigger->mg_pmgDown = gm_pFullScreenCheckBox;
  gm_pFullScreenCheckBox->mg_pmgUp = gm_pResolutionsTrigger;
  gm_pFullScreenCheckBox->mg_pmgDown = gm_pVideoRendering;
  gm_pBitsPerPixelTrigger->mg_pmgUp = gm_pFullScreenCheckBox;
  gm_pBitsPerPixelTrigger->mg_pmgDown = gm_pVideoRendering;
  gm_pVideoRendering->mg_pmgUp = gm_pBitsPerPixelTrigger;
  gm_pVideoRendering->mg_pmgDown = gm_pApply;
  gm_pApply->mg_pmgUp = gm_pVideoRendering;
  gm_pApply->mg_pmgDown = gm_pDisplayAPITrigger;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pDisplayAPITrigger);
  AddChild(gm_pDisplayAdaptersTrigger);
  AddChild(gm_pDisplayPrefsTrigger);
  AddChild(gm_pAspectRatioTrigger);
  AddChild(gm_pResolutionsTrigger);
  AddChild(gm_pFullScreenCheckBox);
  AddChild(gm_pBitsPerPixelTrigger);
  AddChild(gm_pVideoRendering);
  AddChild(gm_pApply);
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
      if (event.GuiEvent.Caller == gm_pVideoRendering) {
        StartRenderingOptionsMenu();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pApply) {
        ApplyVideoOptions();
        return TRUE;
      }
    
    } else if (event.GuiEvent.EventType == EGET_CHANGED) {
    
      if (event.GuiEvent.Caller == gm_pFullScreenCheckBox) {
        UpdateVideoOptionsButtons(0);
        return TRUE;
        
      } else if (event.GuiEvent.Caller == gm_pDisplayPrefsTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;
        
      } else if (event.GuiEvent.Caller == gm_pDisplayAPITrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pDisplayAdaptersTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pAspectRatioTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pResolutionsTrigger) {
        UpdateVideoOptionsButtons(0);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pBitsPerPixelTrigger) {
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