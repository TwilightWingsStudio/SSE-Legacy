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
#include "LevelInfo.h"
#include "MenuStuff.h"
#include "MenuStarters.h"
#include "MSplitStart.h"

extern void UpdateSplitLevel(INDEX iDummy);

// --------------------------------------------------------------------------------------
// Intializes split screen start menu.
// --------------------------------------------------------------------------------------
void CSplitStartMenu::Initialize_t(void)
{
  // Initialize title label
  gm_pTitle = new CMGTitle(TRANS("START SPLIT SCREEN"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Game type" trigger.
  gm_pGameType = new CMGTrigger(TRANS("Game type:"));
  gm_pGameType->mg_strTip = TRANS("choose type of multiplayer game");
  gm_pGameType->mg_boxOnScreen = BoxMediumRow(3);
  gm_pGameType->mg_astrTexts = astrGameTypeRadioTexts;
  gm_pGameType->mg_ctTexts = ctGameTypeRadioTexts;
  gm_pGameType->mg_strValue = astrGameTypeRadioTexts[0];

  // Initialize "Difficulty" trigger.
  gm_pDifficulty = new CMGTrigger(TRANS("Difficulty:"));
  gm_pDifficulty->mg_strTip = TRANS("choose difficulty level");
  gm_pDifficulty->mg_boxOnScreen = BoxMediumRow(3);
  gm_pDifficulty->mg_astrTexts = astrDifficultyRadioTexts;
  gm_pDifficulty->mg_ctTexts = sizeof(astrDifficultyRadioTexts) / sizeof(astrDifficultyRadioTexts[0]);
  gm_pDifficulty->mg_strValue = astrDifficultyRadioTexts[0];

  // Initialize level name
  gm_pLevel->mg_strLabel = TRANS("Level:");
  gm_pLevel->mg_strTip = TRANS("choose the level to start");
  gm_pLevel->mg_strText = "";
  gm_pLevel->mg_boxOnScreen = BoxMediumRow(2);
  gm_pLevel->mg_bfsFontSize = BFS_MEDIUM;
  gm_pLevel->mg_iCenterI = -1;

  // Initialize "Game Options" button
  gm_pGameOptions = new CMGButton(TRANS("Game options"));
  gm_pGameOptions->mg_boxOnScreen = BoxMediumRow(3);
  gm_pGameOptions->mg_bfsFontSize = BFS_MEDIUM;
  gm_pGameOptions->mg_iCenterI = 0;
  gm_pGameOptions->mg_strTip = TRANS("adjust game rules");
  
  // Initialize "Game Mutators" button.
  gm_pGameMutators = new CMGButton(TRANS("Game mutators"));
  gm_pGameMutators->mg_boxOnScreen = BoxMediumRow(4);
  gm_pGameMutators->mg_bfsFontSize = BFS_MEDIUM;
  gm_pGameMutators->mg_iCenterI = 0;
  gm_pGameMutators->mg_strTip = TRANS("adjust game rules");

  // Initialize "Start" button
  gm_pStart = new CMGButton(TRANS("START"));
  gm_pStart->mg_bfsFontSize = BFS_LARGE;
  gm_pStart->mg_boxOnScreen = BoxBigRow(4.5);
  
  // Define neighbours.
  gm_pGameType->mg_pmgUp = gm_pStart;
  gm_pGameType->mg_pmgDown = gm_pDifficulty;
  gm_pDifficulty->mg_pmgUp = gm_pGameType;
  gm_pDifficulty->mg_pmgDown = gm_pLevel;
  gm_pLevel->mg_pmgUp = gm_pDifficulty;
  gm_pLevel->mg_pmgDown = gm_pGameOptions;
  gm_pGameOptions->mg_pmgUp = gm_pLevel;
  gm_pGameOptions->mg_pmgDown = gm_pGameMutators;
  gm_pGameMutators->mg_pmgUp = gm_pGameOptions;
  gm_pGameMutators->mg_pmgDown = gm_pStart;
  gm_pStart->mg_pmgUp = gm_pGameMutators;
  gm_pStart->mg_pmgDown = gm_pGameType;
  
  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pGameType);
  AddChild(gm_pDifficulty);
  AddChild(gm_pLevel);
  AddChild(gm_pGameOptions);
  AddChild(gm_pGameMutators);
  AddChild(gm_pStart);
}

void CSplitStartMenu::StartMenu(void)
{
  extern INDEX sam_bMentalActivated;
  gm_pDifficulty->mg_ctTexts = sam_bMentalActivated ? 7 : 5;

  gm_pGameType->mg_iSelected = Clamp(_pShell->GetINDEX("gam_iStartMode"), 0L, ctGameTypeRadioTexts - 1L);
  gm_pGameType->ApplyCurrentSelection();
  gm_pDifficulty->mg_iSelected = _pShell->GetINDEX("gam_iStartDifficulty") + 1;
  gm_pDifficulty->ApplyCurrentSelection();

  // clamp maximum number of players to at least 4
  _pShell->SetINDEX("gam_ctMaxPlayers", ClampDn(_pShell->GetINDEX("gam_ctMaxPlayers"), 4L));

  UpdateSplitLevel(0);
  CGameMenu::StartMenu();
}

void CSplitStartMenu::EndMenu(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", gm_pDifficulty->mg_iSelected - 1);
  _pShell->SetINDEX("gam_iStartMode", gm_pGameType->mg_iSelected);

  CGameMenu::EndMenu();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CSplitStartMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_TRIGGERED) {
      if (event.GuiEvent.Caller == gm_pLevel) {
        StartSelectLevelFromSplit();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pGameOptions) {
        StartGameOptionsFromSplitScreen();
        return TRUE;
        
      } else if (event.GuiEvent.Caller == gm_pGameMutators) {
        StartVarGameMutators();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pStart) {
        StartSelectPlayersMenuFromSplit();
        return TRUE;
      }

    } else if (event.GuiEvent.EventType == EGET_CHANGED) {

      if (event.GuiEvent.Caller == gm_pGameType) {
        UpdateSplitLevel(-1);
        return TRUE; 
      }
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}