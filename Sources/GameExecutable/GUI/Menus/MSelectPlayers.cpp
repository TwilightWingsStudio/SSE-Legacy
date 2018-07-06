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
#include "MenuStartersAF.h"
#include "MenuManager.h"
#include "MSelectPlayers.h"

#define ADD_GADGET( gd, box, up, dn, lf, rt, txt) \
  gd->mg_boxOnScreen = box; \
  gd->mg_pmgUp = up; \
  gd->mg_pmgDown = dn; \
  gd->mg_pmgLeft = lf; \
  gd->mg_pmgRight = rt; \
  gd->mg_strText = txt; \
  AddChild(&gd);

extern CTString astrNoYes[2];
extern CTString astrSplitScreenRadioTexts[4];
extern void SelectPlayersFillMenu(void);
extern void SelectPlayersApplyMenu(void);

// --------------------------------------------------------------------------------------
// Intializes players selection menu.
// --------------------------------------------------------------------------------------
void CSelectPlayersMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("SELECT PLAYERS"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Dedicated" trigger.
  gm_pDedicated = new CMGTrigger(TRANS("Dedicated:"));
  gm_pDedicated->mg_strTip = TRANS("select to start dedicated server");
  gm_pDedicated->mg_boxOnScreen = BoxMediumRow(0);
  gm_pDedicated->mg_astrTexts = astrNoYes;
  gm_pDedicated->mg_ctTexts = 2;

  // Initialize "Observer" trigger.
  gm_pObserver = new CMGTrigger(TRANS("Observer:"));
  gm_pObserver->mg_strTip = TRANS("select to join in for observing, not for playing");
  gm_pObserver->mg_boxOnScreen = BoxMediumRow(1);
  gm_pObserver->mg_astrTexts = astrNoYes;
  gm_pObserver->mg_ctTexts = 2;

  // Initialize "Number of players" trigger.
  gm_pSplitScreenCfg = new CMGTrigger(TRANS("Number of players:"));
  gm_pSplitScreenCfg->mg_strTip = TRANS("choose more than one player to play in split screen");
  gm_pSplitScreenCfg->mg_boxOnScreen = BoxMediumRow(2);
  gm_pSplitScreenCfg->mg_astrTexts = astrSplitScreenRadioTexts;
  gm_pSplitScreenCfg->mg_ctTexts = sizeof(astrSplitScreenRadioTexts) / sizeof(astrSplitScreenRadioTexts[0]);

  // Initialize players selection buttons.
  gm_pPlayer0Change = new CMGChangePlayer();
  gm_pPlayer1Change = new CMGChangePlayer();
  gm_pPlayer2Change = new CMGChangePlayer();
  gm_pPlayer3Change = new CMGChangePlayer();
  
  gm_pPlayer0Change->mg_iCenterI = -1;
  gm_pPlayer1Change->mg_iCenterI = -1;
  gm_pPlayer2Change->mg_iCenterI = -1;
  gm_pPlayer3Change->mg_iCenterI = -1;

  gm_pPlayer0Change->mg_boxOnScreen = BoxMediumMiddle(4);
  gm_pPlayer1Change->mg_boxOnScreen = BoxMediumMiddle(5);
  gm_pPlayer2Change->mg_boxOnScreen = BoxMediumMiddle(6);
  gm_pPlayer3Change->mg_boxOnScreen = BoxMediumMiddle(7);

  gm_pPlayer0Change->mg_strTip = gm_pPlayer1Change->mg_strTip = gm_pPlayer2Change->mg_strTip = gm_pPlayer3Change->mg_strTip = TRANS("select profile for this player");

  // Initialize notes label.
  gm_pNotes = new CMGButton();
  gm_pNotes->mg_boxOnScreen = BoxMediumRow(9.0);
  gm_pNotes->mg_bfsFontSize = BFS_MEDIUM;
  gm_pNotes->mg_iCenterI = -1;
  gm_pNotes->SetEnabled(FALSE);
  gm_pNotes->mg_bLabel = TRUE;
  gm_pNotes->mg_strText = "";
  
  // Initialize "Start" button.
  gm_pStart = new CMGButton(TRANS("START"));
  gm_pStart->mg_boxOnScreen = BoxMediumRow(11);
  gm_pStart->mg_bfsFontSize = BFS_LARGE;
  gm_pStart->mg_iCenterI = 0;
  
  // Define neighbours.
  gm_pDedicated->mg_pmgUp = gm_pStart;
  gm_pDedicated->mg_pmgDown = gm_pObserver;
  gm_pObserver->mg_pmgUp = gm_pDedicated;
  gm_pObserver->mg_pmgDown = gm_pSplitScreenCfg;
  gm_pSplitScreenCfg->mg_pmgUp = gm_pObserver;
  gm_pSplitScreenCfg->mg_pmgDown = gm_pStart;
  gm_pStart->mg_pmgUp = gm_pSplitScreenCfg;
  gm_pStart->mg_pmgDown = gm_pDedicated;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pDedicated);
  AddChild(gm_pObserver);
  AddChild(gm_pSplitScreenCfg);
  AddChild(gm_pPlayer0Change);
  AddChild(gm_pPlayer1Change);
  AddChild(gm_pPlayer2Change);
  AddChild(gm_pPlayer3Change);
  AddChild(gm_pNotes);
  AddChild(gm_pStart);
}

extern void SelectPlayersFillMenu(void);

static void SelectPlayersApplyMenu(void)
{
  CSelectPlayersMenu &gmCurrent = _pGUIM->gmSelectPlayersMenu;

  if (gmCurrent.gm_bAllowDedicated && gmCurrent.gm_pDedicated->mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_DEDICATED;
    return;
  }

  if (gmCurrent.gm_bAllowObserving && gmCurrent.gm_pObserver->mg_iSelected) {
    _pGame->gm_MenuSplitScreenCfg = CGame::SSC_OBSERVER;
    return;
  }

  _pGame->gm_MenuSplitScreenCfg = (enum CGame::SplitScreenCfg) gmCurrent.gm_pSplitScreenCfg->mg_iSelected;
}

void CSelectPlayersMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
  SelectPlayersFillMenu();
  SelectPlayersApplyMenu();
}

void CSelectPlayersMenu::EndMenu(void)
{
  SelectPlayersApplyMenu();
  CGameMenu::EndMenu();
}

static void UpdateSelectPlayers(INDEX i)
{
  SelectPlayersApplyMenu();
  SelectPlayersFillMenu();
}

extern void StartSplitScreenGame(void);
extern void StartNetworkGame(void);
extern void JoinNetworkGame(void);

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CSelectPlayersMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_TRIGGERED)
    {
      if (event.GuiEvent.Caller == gm_pStart)
      {
        if (gm_pgmParentMenu == &_pGUIM->gmSplitStartMenu) {
          StartSplitScreenGame();

        } else if (gm_pgmParentMenu == &_pGUIM->gmNetworkStartMenu) {
          StartNetworkGame();

        } else if (gm_pgmParentMenu == &_pGUIM->gmLoadSaveMenu) {
          if (_pGUIM->gmLoadSaveMenu.gm_pgmParentMenu == &_pGUIM->gmSplitScreenMenu) {
            StartSplitScreenGameLoad();
          } else {
            StartNetworkLoadGame();
          }

        } else if (gm_pgmParentMenu == &_pGUIM->gmNetworkOpenMenu || gm_pgmParentMenu == &_pGUIM->gmServersMenu) {
          JoinNetworkGame();
        }
        
        return TRUE;
      }
    
    } else if (event.GuiEvent.EventType == EGET_CHANGED) {

      INDEX iNewValue = event.GuiEvent.IntValue;
      
      if (event.GuiEvent.Caller == gm_pDedicated) {
        UpdateSelectPlayers(iNewValue);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pObserver) {
        UpdateSelectPlayers(iNewValue);
        return TRUE;

      } else if (event.GuiEvent.Caller == gm_pSplitScreenCfg) {
        UpdateSelectPlayers(iNewValue);
        return TRUE;
      }
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}