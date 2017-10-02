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
#include "MSinglePlayer.h"
#include "MenuStarters.h"
#include "MenuManager.h"

// --------------------------------------------------------------------------------------
// Intializes single player menu.
// --------------------------------------------------------------------------------------
void CSinglePlayerMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_mgTitle.mg_strText = TRANS("SINGLE PLAYER");
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  // Initialize player label.
  gm_mgPlayerLabel.mg_boxOnScreen = BoxBigRow(-1.0f);
  gm_mgPlayerLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPlayerLabel.mg_iCenterI = -1;
  gm_mgPlayerLabel.mg_bEnabled = FALSE;
  gm_mgPlayerLabel.mg_bLabel = TRUE;

  // Initialize "New Game" button.
  gm_mgNewGame.mg_strText = TRANS("NEW GAME");
  gm_mgNewGame.mg_bfsFontSize = BFS_LARGE;
  gm_mgNewGame.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgNewGame.mg_strTip = TRANS("start new game with current player");
  gm_mgNewGame.mg_pmgUp = &gm_mgOptions;
  gm_mgNewGame.mg_pmgDown = &gm_mgCustom;

  // Initialize "Custom Level" button.
  gm_mgCustom.mg_strText = TRANS("CUSTOM LEVEL");
  gm_mgCustom.mg_bfsFontSize = BFS_LARGE;
  gm_mgCustom.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgCustom.mg_strTip = TRANS("start new game on a custom level");
  gm_mgCustom.mg_pmgUp = &gm_mgNewGame;
  gm_mgCustom.mg_pmgDown = &gm_mgQuickLoad;

  // Initialize "Quick Load" button.
  gm_mgQuickLoad.mg_strText = TRANS("QUICK LOAD");
  gm_mgQuickLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickLoad.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgQuickLoad.mg_strTip = TRANS("load a quick-saved game (F9)");
  gm_mgQuickLoad.mg_pmgUp = &gm_mgCustom;
  gm_mgQuickLoad.mg_pmgDown = &gm_mgLoad;

  // Initialize "Load" button.
  gm_mgLoad.mg_strText = TRANS("LOAD");
  gm_mgLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgLoad.mg_strTip = TRANS("load a saved game of current player");
  gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
  gm_mgLoad.mg_pmgDown = &gm_mgTraining;

  // Initialize "Training" button.
  gm_mgTraining.mg_strText = TRANS("TRAINING");
  gm_mgTraining.mg_bfsFontSize = BFS_LARGE;
  gm_mgTraining.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgTraining.mg_strTip = TRANS("start training level - KarnakDemo");
  gm_mgTraining.mg_pmgUp = &gm_mgLoad;
  gm_mgTraining.mg_pmgDown = &gm_mgTechTest;

  // Initialize "Technology Test" button.
  gm_mgTechTest.mg_strText = TRANS("TECHNOLOGY TEST");
  gm_mgTechTest.mg_bfsFontSize = BFS_LARGE;
  gm_mgTechTest.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgTechTest.mg_strTip = TRANS("start technology testing level");
  gm_mgTechTest.mg_pmgUp = &gm_mgTraining;
  gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;

  // Initialize "Players And Controls" button.
  gm_mgPlayersAndControls.mg_bfsFontSize = BFS_LARGE;
  gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgPlayersAndControls.mg_pmgUp = &gm_mgTechTest;
  gm_mgPlayersAndControls.mg_pmgDown = &gm_mgOptions;
  gm_mgPlayersAndControls.mg_strText = TRANS("PLAYERS AND CONTROLS");
  gm_mgPlayersAndControls.mg_strTip = TRANS("change currently active player or adjust controls");

  // Initialize "Game Options" button.
  gm_mgOptions.mg_strText = TRANS("GAME OPTIONS");
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgOptions.mg_strTip = TRANS("adjust miscellaneous game options");
  gm_mgOptions.mg_pmgUp = &gm_mgPlayersAndControls;
  gm_mgOptions.mg_pmgDown = &gm_mgNewGame;
  
  // Reset pointers.
  gm_mgNewGame.mg_pActivatedFunction = NULL;
  gm_mgCustom.mg_pActivatedFunction = NULL;
  gm_mgQuickLoad.mg_pActivatedFunction = NULL;
  gm_mgLoad.mg_pActivatedFunction = NULL;
  gm_mgTraining.mg_pActivatedFunction = NULL;
  gm_mgTechTest.mg_pActivatedFunction = NULL;
  gm_mgPlayersAndControls.mg_pActivatedFunction = NULL;
  gm_mgOptions.mg_pActivatedFunction = NULL;

  // Add components.
  AddChild(&gm_mgPlayerLabel);
  AddChild(&gm_mgNewGame);
  AddChild(&gm_mgCustom);
  AddChild(&gm_mgQuickLoad);
  AddChild(&gm_mgLoad);
  AddChild(&gm_mgTraining);
  AddChild(&gm_mgTechTest);
  AddChild(&gm_mgPlayersAndControls);
  AddChild(&gm_mgOptions);
}

void CSinglePlayerMenu::StartMenu(void)
{
  gm_mgTraining.mg_bEnabled = IsMenuEnabled("Training");
  gm_mgTechTest.mg_bEnabled = IsMenuEnabled("Technology Test");

  if (gm_mgTraining.mg_bEnabled) {
    if (!gm_mgTraining.mg_lnNode.IsLinked()) {
      AddChild(&gm_mgTraining);
    }

    gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
    gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
    gm_mgLoad.mg_pmgDown = &gm_mgTraining;

    gm_mgTraining.mg_boxOnScreen = BoxBigRow(4.0f);
    gm_mgTraining.mg_pmgUp = &gm_mgLoad;
    gm_mgTraining.mg_pmgDown = &gm_mgTechTest;

    gm_mgTechTest.mg_boxOnScreen = BoxBigRow(5.0f);
    gm_mgTechTest.mg_pmgUp = &gm_mgTraining;
    gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;

    gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(6.0f);
    gm_mgOptions.mg_boxOnScreen = BoxBigRow(7.0f);

  } else {
    if (gm_mgTraining.mg_lnNode.IsLinked()) {
      gm_mgTraining.mg_lnNode.Remove();
    }

    gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
    gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
    gm_mgLoad.mg_pmgDown = &gm_mgTechTest;

    gm_mgTechTest.mg_boxOnScreen = BoxBigRow(4.0f);
    gm_mgTechTest.mg_pmgUp = &gm_mgLoad;
    gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;

    gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(5.0f);
    gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  }

  CGameMenu::StartMenu();

  CPlayerCharacter &pc = _pGame->gm_apcPlayers[_pGame->gm_iSinglePlayer];
  gm_mgPlayerLabel.mg_strText.PrintF(TRANS("Player: %s\n"), pc.GetNameForPrinting());
}

extern CTString sam_strTechTestLevel;
extern CTString sam_strTrainingLevel;

static void StartTechTest(void)
{
  _pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  _pGame->gam_strCustomLevel = sam_strTechTestLevel;

  _pShell->SetINDEX("gam_iStartDifficulty", CSessionProperties::GD_NORMAL);
  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);

  extern void StartSinglePlayerGame(void);
  StartSinglePlayerGame();
}

static void StartTraining(void)
{
  _pGUIM->gmSinglePlayerNewMenu.gm_pgmParentMenu = &_pGUIM->gmSinglePlayerMenu;
  _pGame->gam_strCustomLevel = sam_strTrainingLevel;
  ChangeToMenu(&_pGUIM->gmSinglePlayerNewMenu);
}

// [SSE]
BOOL CSinglePlayerMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == &gm_mgNewGame) {
      StartSinglePlayerNewMenu();

    } else if (event.GuiEvent.Caller == &gm_mgCustom) {
      StartSelectLevelFromSingle();

    } else if (event.GuiEvent.Caller == &gm_mgQuickLoad) {
      StartSinglePlayerQuickLoadMenu();

    } else if (event.GuiEvent.Caller == &gm_mgLoad) {
      StartSinglePlayerLoadMenu();

    } else if (event.GuiEvent.Caller == &gm_mgTraining) {
      StartTraining();

    } else if (event.GuiEvent.Caller == &gm_mgTechTest) {
      StartTechTest();

    } else if (event.GuiEvent.Caller == &gm_mgPlayersAndControls) {
      StartChangePlayerMenuFromSinglePlayer();

    } else if (event.GuiEvent.Caller == &gm_mgOptions) {
      StartSinglePlayerGameOptions();
    }
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}