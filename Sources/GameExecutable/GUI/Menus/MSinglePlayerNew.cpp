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
#include "MSinglePlayerNew.h"

// --------------------------------------------------------------------------------------
// Intializes single player new menu.
// --------------------------------------------------------------------------------------
void CSinglePlayerNewMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("SPLIT SCREEN"));
  gm_pTitle->mg_strText = TRANS("NEW GAME");
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Tourist" button.
  gm_pTourist = new CMGButton(TRANS("TOURIST"));
  gm_pTourist->mg_bfsFontSize = BFS_LARGE;
  gm_pTourist->mg_boxOnScreen = BoxBigRow(0.0f);
  gm_pTourist->mg_strTip = TRANS("for non-FPS players");

  // Initialize "Easy" button.
  gm_pEasy = new CMGButton(TRANS("EASY"));
  gm_pEasy->mg_bfsFontSize = BFS_LARGE;
  gm_pEasy->mg_boxOnScreen = BoxBigRow(1.0f);
  gm_pEasy->mg_strTip = TRANS("for unexperienced FPS players");

  // Initialize "Medium" button.
  gm_pMedium = new CMGButton(TRANS("NORMAL"));
  gm_pMedium->mg_bfsFontSize = BFS_LARGE;
  gm_pMedium->mg_boxOnScreen = BoxBigRow(2.0f);
  gm_pMedium->mg_strTip = TRANS("for experienced FPS players");

  // Initialize "Hard" button.
  gm_pHard = new CMGButton(TRANS("HARD"));
  gm_pHard->mg_bfsFontSize = BFS_LARGE;
  gm_pHard->mg_boxOnScreen = BoxBigRow(3.0f);
  gm_pHard->mg_strTip = TRANS("for experienced Serious Sam players");

  // Initialize "Serious" button.
  gm_pSerious = new CMGButton(TRANS("SERIOUS"));
  gm_pSerious->mg_bfsFontSize = BFS_LARGE;
  gm_pSerious->mg_boxOnScreen = BoxBigRow(4.0f);
  gm_pSerious->mg_strTip = TRANS("are you serious?");

  // Initialize "Mental" button.
  gm_pMental = new CMGButton(TRANS("MENTAL"));
  gm_pMental->mg_bfsFontSize = BFS_LARGE;
  gm_pMental->mg_boxOnScreen = BoxBigRow(5.0f);
  gm_pMental->mg_strTip = TRANS("you are not serious!");
  gm_pMental->mg_bBlinking = FALSE;
  
  // Initialize "Serious Mental" button.
  gm_pSeriousMental->mg_strText = TRANS("^cFF0000SERIOUS MENTAL");
  gm_pSeriousMental->mg_bfsFontSize = BFS_LARGE;
  gm_pSeriousMental->mg_boxOnScreen = BoxBigRow(6.0f);
  gm_pSeriousMental->mg_strTip = TRANS("you are too serious!");
  gm_pSeriousMental->mg_bBlinking = TRUE;

  // Define neighbours.
  gm_pTourist->mg_pmgUp = gm_pSerious;
  gm_pTourist->mg_pmgDown = gm_pEasy;
  gm_pEasy->mg_pmgUp = gm_pTourist;
  gm_pEasy->mg_pmgDown = gm_pMedium;
  gm_pMedium->mg_pmgUp = gm_pEasy;
  gm_pMedium->mg_pmgDown = gm_pHard;
  gm_pHard->mg_pmgUp = gm_pMedium;
  gm_pHard->mg_pmgDown = gm_pSerious;
  gm_pSerious->mg_pmgUp = gm_pHard;
  gm_pSerious->mg_pmgDown = gm_pTourist;
  gm_pMental->mg_pmgUp = gm_pSerious;
  gm_pMental->mg_pmgDown = gm_pTourist;
  gm_pSeriousMental->mg_pmgUp = gm_pMental;
  gm_pSeriousMental->mg_pmgDown = gm_pTourist;
  
  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pTourist);
  AddChild(gm_pEasy);
  AddChild(gm_pMedium);
  AddChild(gm_pHard);
  AddChild(gm_pSerious);
  AddChild(gm_pMental);
  AddChild(gm_pSeriousMental);
}

void CSinglePlayerNewMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
  extern INDEX sam_bMentalActivated;

  if (sam_bMentalActivated) {
    gm_pMental->Appear();
    gm_pSeriousMental->Appear();
    gm_pSerious->mg_pmgDown = gm_pMental;
    gm_pTourist->mg_pmgUp = gm_pMental;
    
    // [SSE] Serious Mental
    gm_pMental->mg_pmgDown = gm_pSeriousMental;
    gm_pTourist->mg_pmgUp = gm_pSeriousMental;
    //
    
  } else {
    gm_pMental->Disappear();
    gm_pSeriousMental->Disappear(); // [SSE] Serious Mental

    gm_pSerious->mg_pmgDown = gm_pTourist;
    gm_pTourist->mg_pmgUp = gm_pSerious;
  }
}

// [SSE]
BOOL CSinglePlayerNewMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    INDEX iDifficulty = -666;
    
    if (event.GuiEvent.Caller == gm_pTourist) {
      iDifficulty = CSessionProperties::GD_TOURIST;

    } else if (event.GuiEvent.Caller == gm_pEasy) {
      iDifficulty = CSessionProperties::GD_EASY;

    } else if (event.GuiEvent.Caller == gm_pMedium) {
      iDifficulty = CSessionProperties::GD_NORMAL;

    } else if (event.GuiEvent.Caller == gm_pHard) {
      iDifficulty = CSessionProperties::GD_HARD;

    } else if (event.GuiEvent.Caller == gm_pSerious) {
      iDifficulty = CSessionProperties::GD_EXTREME;
    
    } else if (event.GuiEvent.Caller == gm_pMental) {
      iDifficulty = CSessionProperties::GD_EXTREME + 1;

    } else if (event.GuiEvent.Caller == gm_pSeriousMental) {
      iDifficulty = CSessionProperties::GD_EXTREME + 2;
    }

    if (iDifficulty >= CSessionProperties::GD_TOURIST) {
      _pShell->SetINDEX("gam_iStartDifficulty", iDifficulty);
      _pShell->SetINDEX("gam_iStartMode", CSessionProperties::GM_COOPERATIVE);
      
      extern void StartSinglePlayerGame(void);
      StartSinglePlayerGame();
      
      return TRUE;
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}
