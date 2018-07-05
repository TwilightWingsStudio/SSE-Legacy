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
#include "MSplitScreen.h"

// --------------------------------------------------------------------------------------
// Intializes split screen menu.
// --------------------------------------------------------------------------------------
void CSplitScreenMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("SPLIT SCREEN"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "New Game" button.
  gm_pStartButton = new CMGButton(TRANS("NEW GAME"));
  gm_pStartButton->mg_bfsFontSize = BFS_LARGE;
  gm_pStartButton->mg_boxOnScreen = BoxBigRow(0);
  gm_pStartButton->mg_strTip = TRANS("start new split-screen game");

  // Initialize "Quick Load" button.
  gm_pQuickLoadButton = new CMGButton(TRANS("QUICK LOAD"));
  gm_pQuickLoadButton->mg_bfsFontSize = BFS_LARGE;
  gm_pQuickLoadButton->mg_boxOnScreen = BoxBigRow(1);
  gm_pQuickLoadButton->mg_strTip = TRANS("load a quick-saved game (F9)");

  // Initialize "Load" button.
  gm_pLoadButton = new CMGButton(TRANS("LOAD"));
  gm_pLoadButton->mg_bfsFontSize = BFS_LARGE;
  gm_pLoadButton->mg_boxOnScreen = BoxBigRow(2);
  gm_pLoadButton->mg_strTip = TRANS("load a saved split-screen game");
  
  // Define neighbours.
  gm_pStartButton->mg_pmgUp = gm_pLoadButton;
  gm_pStartButton->mg_pmgDown = gm_pQuickLoadButton;
  gm_pQuickLoadButton->mg_pmgUp = gm_pStartButton;
  gm_pQuickLoadButton->mg_pmgDown = gm_pLoadButton;
  gm_pLoadButton->mg_pmgUp = gm_pQuickLoadButton;
  gm_pLoadButton->mg_pmgDown = gm_pStartButton;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pStartButton);
  AddChild(gm_pQuickLoadButton);
  AddChild(gm_pLoadButton);
}

void CSplitScreenMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CSplitScreenMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == gm_pStartButton) {
      StartSplitStartMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == gm_pQuickLoadButton) {
      StartSplitScreenQuickLoadMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == gm_pLoadButton) {
      StartSplitScreenLoadMenu();
      return TRUE;
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}