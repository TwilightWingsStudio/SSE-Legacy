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
#include "MNetworkStart.h"

extern INDEX sam_bMentalActivated;

extern void UpdateNetworkLevel(INDEX iDummy);

// --------------------------------------------------------------------------------------
// Intializes network start menu.
// --------------------------------------------------------------------------------------
void CNetworkStartMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("START SERVER"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Session name" edit box.
  gm_pSessionName = new CMGEdit();
  gm_pSessionName->mg_strText = _pGame->gam_strSessionName;
  gm_pSessionName->mg_strLabel = TRANS("Session name:");
  gm_pSessionName->mg_strTip = TRANS("name the session to start");
  gm_pSessionName->mg_ctMaxStringLen = 25;
  gm_pSessionName->mg_pstrToChange = &_pGame->gam_strSessionName;
  gm_pSessionName->mg_boxOnScreen = BoxMediumRow(1);
  gm_pSessionName->mg_bfsFontSize = BFS_MEDIUM;
  gm_pSessionName->mg_iCenterI = -1;

  // Initialize "Game type" trigger.
  gm_pGameType = new CMGTrigger(TRANS("Game type:"));
  gm_pGameType->mg_strTip = TRANS("choose type of multiplayer game");
  gm_pGameType->mg_boxOnScreen = BoxMediumRow(2);
  gm_pGameType->mg_astrTexts = astrGameTypeRadioTexts;
  gm_pGameType->mg_ctTexts = ctGameTypeRadioTexts;

  // Initialize "Difficulty" trigger.
  gm_pDifficulty = new CMGTrigger(TRANS("Difficulty:"));
  gm_pDifficulty->mg_strTip = TRANS("choose difficulty level");
  gm_pDifficulty->mg_boxOnScreen = BoxMediumRow(3);
  gm_pDifficulty->mg_astrTexts = astrDifficultyRadioTexts;
  gm_pDifficulty->mg_ctTexts = sizeof(astrDifficultyRadioTexts) / sizeof(astrDifficultyRadioTexts[0]);

  // Initialize "Level" button.
  gm_pLevel = new CMGButton();
  gm_pLevel->mg_strText = "";
  gm_pLevel->mg_strLabel = TRANS("Level:");
  gm_pLevel->mg_strTip = TRANS("choose the level to start");
  gm_pLevel->mg_boxOnScreen = BoxMediumRow(4);
  gm_pLevel->mg_bfsFontSize = BFS_MEDIUM;
  gm_pLevel->mg_iCenterI = -1;
  
  // Initialize "Min players" trigger.
  gm_pMinPlayers = new CMGTrigger(TRANS("Min players:"));
  gm_pMinPlayers->mg_strTip = TRANS("choose minimum needed number of players");
  gm_pMinPlayers->mg_boxOnScreen = BoxMediumRow(5);
  gm_pMinPlayers->mg_astrTexts = astrMinPlayersRadioTexts;
  gm_pMinPlayers->mg_ctTexts = 16;

  // Initialize "Max players" trigger.
  gm_pMaxPlayers = new CMGTrigger(TRANS("Max players:"));
  gm_pMaxPlayers->mg_strTip = TRANS("choose maximum allowed number of players");
  gm_pMaxPlayers->mg_boxOnScreen = BoxMediumRow(6);
  gm_pMaxPlayers->mg_astrTexts = astrMaxPlayersRadioTexts;
  gm_pMaxPlayers->mg_ctTexts = 16;

  // Initialize "Wait all players" trigger.
  //TRIGGER_MG(gm_pWaitAllPlayers, 6,
  //  gm_pMaxPlayers, gm_pVisible, TRANS("Wait for all players:"), astrNoYes);
  //gm_pWaitAllPlayers->mg_strTip = TRANS("if on, game won't start until all players have joined");

  // Initialize "Server visible" trigger.
  gm_pVisible = new CMGTrigger(TRANS("Server visible:"));
  gm_pVisible->mg_strTip = TRANS("invisible servers are not listed, cleints have to join manually");
  gm_pVisible->mg_boxOnScreen = BoxMediumRow(7);
  gm_pVisible->mg_astrTexts = astrNoYes;
  gm_pVisible->mg_ctTexts = 2;

  // Initialize "Game options" button.
  gm_pGameOptions = new CMGButton(TRANS("Game options"));
  gm_pGameOptions->mg_strTip = TRANS("adjust game rules");
  gm_pGameOptions->mg_boxOnScreen = BoxMediumRow(8);
  gm_pGameOptions->mg_bfsFontSize = BFS_MEDIUM;
  gm_pGameOptions->mg_iCenterI = 0;
  
  // Initialize "Game mutators" button.
  gm_pGameMutators = new CMGButton(TRANS("Game mutators"));
  gm_pGameMutators->mg_strTip = TRANS("adjust game rules");
  gm_pGameMutators->mg_boxOnScreen = BoxMediumRow(9);
  gm_pGameMutators->mg_bfsFontSize = BFS_MEDIUM;
  gm_pGameMutators->mg_iCenterI = 0;

  // Initialize "Start" button.
  gm_pStart = new CMGButton(TRANS("START"));
  gm_pStart->mg_boxOnScreen = BoxBigRow(7.5);
  gm_pStart->mg_bfsFontSize = BFS_LARGE;

  // Define neighbours.
  gm_pSessionName->mg_pmgUp = gm_pStart;
  gm_pSessionName->mg_pmgDown = gm_pGameType;
  gm_pGameType->mg_pmgUp = gm_pSessionName;
  gm_pGameType->mg_pmgDown = gm_pDifficulty;
  gm_pDifficulty->mg_pmgUp = gm_pGameType;
  gm_pDifficulty->mg_pmgDown = gm_pLevel;
  gm_pLevel->mg_pmgUp = gm_pDifficulty;
  gm_pLevel->mg_pmgDown = gm_pMinPlayers;
  gm_pMinPlayers->mg_pmgUp = gm_pLevel;
  gm_pMinPlayers->mg_pmgDown = gm_pMaxPlayers;
  gm_pMaxPlayers->mg_pmgUp = gm_pMinPlayers;
  gm_pMaxPlayers->mg_pmgDown = gm_pVisible;
  gm_pVisible->mg_pmgUp = gm_pMaxPlayers;
  gm_pVisible->mg_pmgDown = gm_pGameOptions;
  gm_pGameOptions->mg_pmgUp = gm_pVisible;
  gm_pGameOptions->mg_pmgDown = gm_pGameMutators;
  gm_pGameMutators->mg_pmgUp = gm_pGameOptions;
  gm_pGameMutators->mg_pmgDown = gm_pStart;
  gm_pStart->mg_pmgUp = gm_pGameMutators;
  gm_pStart->mg_pmgDown = gm_pSessionName;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pSessionName);
  AddChild(gm_pGameType);
  AddChild(gm_pDifficulty);
  AddChild(gm_pLevel);
  AddChild(gm_pMinPlayers);
  AddChild(gm_pMaxPlayers);
  AddChild(gm_pVisible);
  AddChild(gm_pGameOptions);
  AddChild(gm_pGameMutators);
  AddChild(gm_pStart);
}

void CNetworkStartMenu::StartMenu(void)
{
  gm_pDifficulty->mg_ctTexts = sam_bMentalActivated ? 7 : 5;

  gm_pGameType->mg_iSelected = Clamp(_pShell->GetINDEX("gam_iStartMode"), 0L, ctGameTypeRadioTexts - 1L);
  gm_pDifficulty->mg_iSelected = _pShell->GetINDEX("gam_iStartDifficulty") + 1;
  gm_pGameType->ApplyCurrentSelection();
  gm_pDifficulty->ApplyCurrentSelection();

  _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);

  INDEX ctMaxPlayers = _pShell->GetINDEX("gam_ctMaxPlayers");
  if (ctMaxPlayers<2 || ctMaxPlayers>16) {
    ctMaxPlayers = 2;
    _pShell->SetINDEX("gam_ctMaxPlayers", ctMaxPlayers);
  }
  
  gm_pMinPlayers->mg_iSelected = 0;
  gm_pMaxPlayers->mg_iSelected = ctMaxPlayers - 2;

  gm_pMinPlayers->ApplyCurrentSelection();
  gm_pMaxPlayers->ApplyCurrentSelection();

  //gm_pWaitAllPlayers->mg_iSelected = Clamp(_pShell->GetINDEX("gam_bWaitAllPlayers"), 0L, 1L);
  //gm_pWaitAllPlayers->ApplyCurrentSelection();

  gm_pVisible->mg_iSelected = _pShell->GetINDEX("ser_bEnumeration");
  gm_pVisible->ApplyCurrentSelection();

  UpdateNetworkLevel(0);

  CGameMenu::StartMenu();
}

void CNetworkStartMenu::EndMenu(void)
{
  _pShell->SetINDEX("gam_iStartDifficulty", gm_pDifficulty->mg_iSelected - 1);
  _pShell->SetINDEX("gam_iStartMode", gm_pGameType->mg_iSelected);
  //_pShell->SetINDEX("gam_bWaitAllPlayers", gm_pWaitAllPlayers->mg_iSelected);
  _pShell->SetINDEX("gam_ctMaxPlayers", gm_pMaxPlayers->mg_iSelected + 2);
  _pShell->SetINDEX("ser_bEnumeration", gm_pVisible->mg_iSelected);

  CGameMenu::EndMenu();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CNetworkStartMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_TRIGGERED) {
      if (event.GuiEvent.Caller == gm_pLevel) {
        StartSelectLevelFromNetwork();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pGameOptions) {
        StartGameOptionsFromNetwork();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pGameMutators) {
        StartVarGameMutators();
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pStart) {
        StartSelectPlayersMenuFromNetwork();
        return TRUE;
      }
      
    } else if (event.GuiEvent.EventType == EGET_CHANGED) {

      if (event.GuiEvent.Caller == gm_pGameType) {
        UpdateNetworkLevel(-1);
        return TRUE; 
      }
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}
