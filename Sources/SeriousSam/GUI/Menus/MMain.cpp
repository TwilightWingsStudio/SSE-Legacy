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
#include "MMain.h"

extern CTString sam_strVersion;
extern CTString sam_strBuildDate;
extern CTString sam_strModName;

// --------------------------------------------------------------------------------------
// Intializes main menu.
// --------------------------------------------------------------------------------------
void CMainMenu::Initialize_t(void)
{
  // intialize main menu
  /*
  gm_mgTitle.mg_strText = "SERIOUS SAM - BETA";  // nothing to see here, kazuya
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);
  */

  // Initialize version label.
  gm_mgVersionLabel.mg_strText = sam_strVersion;
  gm_mgVersionLabel.mg_boxOnScreen = BoxVersion(-5.5F);
  gm_mgVersionLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgVersionLabel.mg_iCenterI = +1;
  gm_mgVersionLabel.mg_bEnabled = FALSE;
  gm_mgVersionLabel.mg_bLabel = TRUE;
  
  // Initialize build date label.
  gm_mgBuildDateLabel.mg_strText = sam_strBuildDate;
  gm_mgBuildDateLabel.mg_boxOnScreen = BoxVersion(-4.5F);
  gm_mgBuildDateLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgBuildDateLabel.mg_iCenterI = +1;
  gm_mgBuildDateLabel.mg_bEnabled = FALSE;
  gm_mgBuildDateLabel.mg_bLabel = TRUE;

  // Initialize mod name label.
  gm_mgModLabel.mg_strText = sam_strModName;
  gm_mgModLabel.mg_boxOnScreen = BoxMediumRow(-2.0f);
  gm_mgModLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgModLabel.mg_iCenterI = 0;
  gm_mgModLabel.mg_bEnabled = FALSE;
  gm_mgModLabel.mg_bLabel = TRUE;

  // Initialize "Single Player" button.
  gm_mgSingle.mg_strText = TRANS("SINGLE PLAYER");
  gm_mgSingle.mg_bfsFontSize = BFS_LARGE;
  gm_mgSingle.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgSingle.mg_strTip = TRANS("single player game menus");
  gm_mgSingle.mg_pmgUp = &gm_mgQuit;
  gm_mgSingle.mg_pmgDown = &gm_mgNetwork;

  // Initialize "Network" button.
  gm_mgNetwork.mg_strText = TRANS("NETWORK");
  gm_mgNetwork.mg_bfsFontSize = BFS_LARGE;
  gm_mgNetwork.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgNetwork.mg_strTip = TRANS("LAN/iNet multiplayer menus");
  gm_mgNetwork.mg_pmgUp = &gm_mgSingle;
  gm_mgNetwork.mg_pmgDown = &gm_mgSplitScreen;

  // Initialize "Split Screen" button.
  gm_mgSplitScreen.mg_strText = TRANS("SPLIT SCREEN");
  gm_mgSplitScreen.mg_bfsFontSize = BFS_LARGE;
  gm_mgSplitScreen.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgSplitScreen.mg_strTip = TRANS("play with multiple players on one computer");
  gm_mgSplitScreen.mg_pmgUp = &gm_mgNetwork;
  gm_mgSplitScreen.mg_pmgDown = &gm_mgDemo;

  // Initialize "Demo" button.
  gm_mgDemo.mg_strText = TRANS("DEMO");
  gm_mgDemo.mg_bfsFontSize = BFS_LARGE;
  gm_mgDemo.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgDemo.mg_strTip = TRANS("play a game demo");
  gm_mgDemo.mg_pmgUp = &gm_mgSplitScreen;
  gm_mgDemo.mg_pmgDown = &gm_mgMods;

  // Initialize "Mods" button.
  gm_mgMods.mg_strText = TRANS("MODS");
  gm_mgMods.mg_bfsFontSize = BFS_LARGE;
  gm_mgMods.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgMods.mg_strTip = TRANS("run one of installed game modifications");
  gm_mgMods.mg_pmgUp = &gm_mgDemo;
  gm_mgMods.mg_pmgDown = &gm_mgHighScore;

  // Initialize "High Scores" button.
  gm_mgHighScore.mg_strText = TRANS("HIGH SCORES");
  gm_mgHighScore.mg_bfsFontSize = BFS_LARGE;
  gm_mgHighScore.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgHighScore.mg_strTip = TRANS("view list of top ten best scores");
  gm_mgHighScore.mg_pmgUp = &gm_mgMods;
  gm_mgHighScore.mg_pmgDown = &gm_mgOptions;

  // Initialize "Options" button.
  gm_mgOptions.mg_strText = TRANS("OPTIONS");
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgOptions.mg_strTip = TRANS("adjust video, audio and input options");
  gm_mgOptions.mg_pmgUp = &gm_mgHighScore;
  gm_mgOptions.mg_pmgDown = &gm_mgQuit;

  // Initialize "Quit" button.
  gm_mgQuit.mg_strText = TRANS("QUIT");
  gm_mgQuit.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuit.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgQuit.mg_strTip = TRANS("exit game immediately");
  gm_mgQuit.mg_pmgUp = &gm_mgOptions;
  gm_mgQuit.mg_pmgDown = &gm_mgSingle;
  
  // Reset pointers.
  gm_mgSingle.mg_pActivatedFunction = NULL;
  gm_mgNetwork.mg_pActivatedFunction = NULL;
  gm_mgSplitScreen.mg_pActivatedFunction = NULL;
  gm_mgDemo.mg_pActivatedFunction = NULL;
  gm_mgMods.mg_pActivatedFunction = NULL;
  gm_mgHighScore.mg_pActivatedFunction = NULL;
  gm_mgOptions.mg_pActivatedFunction = NULL;
  gm_mgQuit.mg_pActivatedFunction = NULL;

  // Add components.
  AddChild(&gm_mgVersionLabel);
  AddChild(&gm_mgBuildDateLabel);
  AddChild(&gm_mgModLabel);

  AddChild(&gm_mgSingle);
  AddChild(&gm_mgNetwork);
  AddChild(&gm_mgSplitScreen);
  AddChild(&gm_mgDemo);
  AddChild(&gm_mgMods);
  AddChild(&gm_mgHighScore);
  AddChild(&gm_mgOptions);
  AddChild(&gm_mgQuit);
}

void CMainMenu::StartMenu(void)
{
  gm_mgSingle.mg_bEnabled = IsMenuEnabled("Single Player");
  gm_mgNetwork.mg_bEnabled = IsMenuEnabled("Network");
  gm_mgSplitScreen.mg_bEnabled = IsMenuEnabled("Split Screen");
  gm_mgHighScore.mg_bEnabled = IsMenuEnabled("High Score");
  CGameMenu::StartMenu();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CMainMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == &gm_mgSingle) {
      StartSinglePlayerMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == &gm_mgNetwork) {
      StartNetworkMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgSplitScreen) {
      StartSplitScreenMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgDemo) {
      StartDemoLoadMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgMods) {
      StartModsLoadMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgHighScore) {
      StartHighScoreMenu();
      return TRUE;
      
    } else if (event.GuiEvent.Caller == &gm_mgOptions) {
      StartOptionsMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == &gm_mgQuit) {
      extern void ExitConfirm(void);
      ExitConfirm();
      return TRUE;
    }
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}