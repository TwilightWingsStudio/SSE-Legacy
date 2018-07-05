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
#include "MControls.h"

extern CTFileName _fnmControlsToCustomize;

// --------------------------------------------------------------------------------------
// Intializes controls menu.
// --------------------------------------------------------------------------------------
void CControlsMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("CONTROLS"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize player name label.
  gm_pNameLabel = new CMGButton();
  gm_pNameLabel->mg_boxOnScreen = BoxMediumRow(0.0);
  gm_pNameLabel->mg_bfsFontSize = BFS_MEDIUM;
  gm_pNameLabel->mg_iCenterI = -1;
  gm_pNameLabel->SetEnabled(FALSE);
  gm_pNameLabel->mg_bLabel = TRUE;

  // Initialize "Customize Controls" button.
  gm_pButtons = new CMGButton(TRANS("CUSTOMIZE BUTTONS"));
  gm_pButtons->mg_strTip = TRANS("customize buttons in current controls");
  gm_pButtons->mg_boxOnScreen = BoxMediumRow(2.0);
  gm_pButtons->mg_bfsFontSize = BFS_MEDIUM;
  gm_pButtons->mg_iCenterI = 0;

  // Initialize "Advanced Joysrick Setup" button.
  gm_pAdvanced = new CMGButton(TRANS("ADVANCED JOYSTICK SETUP"));
  gm_pAdvanced->mg_strTip = TRANS("adjust advanced settings for joystick axis");
  gm_pAdvanced->mg_boxOnScreen = BoxMediumRow(3);
  gm_pAdvanced->mg_iCenterI = 0;
  gm_pAdvanced->mg_bfsFontSize = BFS_MEDIUM;

  // Initialize "Sensitivity" slider.
  gm_pSensitivity = new CMGSlider(TRANS("SENSITIVITY"));
  gm_pSensitivity->mg_strTip = TRANS("sensitivity for all axis in this control set");
  gm_pSensitivity->mg_boxOnScreen = BoxMediumRow(4.5);
  
  // Initialize "INVERT LOOK" trigger.
  gm_pInvertTrigger = new CMGTrigger(TRANS("INVERT LOOK"));
  gm_pInvertTrigger->mg_strTip = TRANS("invert up/down looking");
  gm_pInvertTrigger->mg_boxOnScreen = BoxMediumRow(5.5);
  gm_pInvertTrigger->mg_astrTexts = astrNoYes;
  gm_pInvertTrigger->mg_ctTexts = 2;
  gm_pInvertTrigger->mg_strValue = astrNoYes[0];
  
  // Initialize "INVERT LOOK" trigger.
  gm_pSmoothTrigger = new CMGTrigger(TRANS("SMOOTH AXIS"));
  gm_pSmoothTrigger->mg_strTip = TRANS("smooth mouse/joystick movements");
  gm_pSmoothTrigger->mg_boxOnScreen = BoxMediumRow(6.5);
  gm_pSmoothTrigger->mg_astrTexts = astrNoYes;
  gm_pSmoothTrigger->mg_ctTexts = 2;
  gm_pSmoothTrigger->mg_strValue = astrNoYes[0];
  
  // Initialize "Mouse Acceleration" trigger.
  gm_pAccelTrigger = new CMGTrigger(TRANS("MOUSE ACCELERATION"));
  gm_pAccelTrigger->mg_strTip = TRANS("allow mouse acceleration");
  gm_pAccelTrigger->mg_boxOnScreen = BoxMediumRow(7.5);
  gm_pAccelTrigger->mg_astrTexts = astrNoYes;
  gm_pAccelTrigger->mg_ctTexts = 2;
  gm_pAccelTrigger->mg_strValue = astrNoYes[0];

  // Initialize "Enable IFeel" trigger.
  gm_pIFeelTrigger = new CMGTrigger(TRANS("ENABLE IFEEL"));
  gm_pIFeelTrigger->mg_strTip = TRANS("enable support for iFeel tactile feedback mouse");
  gm_pIFeelTrigger->mg_boxOnScreen = BoxMediumRow(8.5);
  gm_pIFeelTrigger->mg_astrTexts = astrNoYes;
  gm_pIFeelTrigger->mg_ctTexts = 2;
  gm_pIFeelTrigger->mg_strValue = astrNoYes[0];
  
  // Initialize "Load Predefined Settings" button.
  gm_pPredefined = new CMGButton(TRANS("LOAD PREDEFINED SETTINGS"));
  gm_pPredefined->mg_strTip = TRANS("load one of several predefined control settings");
  gm_pPredefined->mg_boxOnScreen = BoxMediumRow(10);
  gm_pPredefined->mg_iCenterI = 0;
  gm_pPredefined->mg_bfsFontSize = BFS_MEDIUM;
  
  // Define neighbours.
  gm_pButtons->mg_pmgUp = gm_pPredefined;
  gm_pButtons->mg_pmgDown = gm_pAdvanced;
  gm_pAdvanced->mg_pmgUp = gm_pButtons;
  gm_pAdvanced->mg_pmgDown = gm_pSensitivity;
  gm_pSensitivity->mg_pmgUp = gm_pAdvanced;
  gm_pSensitivity->mg_pmgDown = gm_pInvertTrigger;
  gm_pInvertTrigger->mg_pmgUp = gm_pSensitivity;
  gm_pInvertTrigger->mg_pmgDown = gm_pSmoothTrigger;
  gm_pSmoothTrigger->mg_pmgUp = gm_pInvertTrigger;
  gm_pSmoothTrigger->mg_pmgDown = gm_pAccelTrigger;
  gm_pAccelTrigger->mg_pmgUp = gm_pSmoothTrigger;
  gm_pAccelTrigger->mg_pmgDown = gm_pIFeelTrigger;
  gm_pIFeelTrigger->mg_pmgUp = gm_pAccelTrigger;
  gm_pIFeelTrigger->mg_pmgDown = gm_pPredefined;
  gm_pPredefined->mg_pmgUp = gm_pIFeelTrigger;
  gm_pPredefined->mg_pmgDown = gm_pButtons;

  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pNameLabel);
  AddChild(gm_pButtons);
  AddChild(gm_pAdvanced);
  AddChild(gm_pSensitivity);
  AddChild(gm_pInvertTrigger);
  AddChild(gm_pSmoothTrigger);
  AddChild(gm_pAccelTrigger);
  AddChild(gm_pIFeelTrigger);
  AddChild(gm_pPredefined);
}

void CControlsMenu::StartMenu(void)
{
  gm_pmgSelectedByDefault = gm_pButtons;
  INDEX iPlayer = _pGame->gm_iSinglePlayer;

  if (_iLocalPlayer >= 0 && _iLocalPlayer<4) {
    iPlayer = _pGame->gm_aiMenuLocalPlayers[_iLocalPlayer];
  }

  _fnmControlsToCustomize.PrintF("Controls\\Controls%d.ctl", iPlayer);

  ControlsMenuOn();

  gm_pNameLabel->mg_strText.PrintF(TRANS("CONTROLS FOR: %s"), _pGame->gm_apcPlayers[iPlayer].GetNameForPrinting());

  ObtainActionSettings();
  CGameMenu::StartMenu();
}

void CControlsMenu::EndMenu(void)
{
  ApplyActionSettings();

  ControlsMenuOff();

  CGameMenu::EndMenu();
}

void CControlsMenu::ObtainActionSettings(void)
{
  CControls &ctrls = _pGame->gm_ctrlControlsExtra;

  gm_pSensitivity->mg_iMinPos = 0;
  gm_pSensitivity->mg_iMaxPos = 50;
  gm_pSensitivity->mg_iCurPos = ctrls.ctrl_fSensitivity / 2;
  gm_pSensitivity->ApplyCurrentPosition();

  gm_pInvertTrigger->mg_iSelected = ctrls.ctrl_bInvertLook ? 1 : 0;
  gm_pSmoothTrigger->mg_iSelected = ctrls.ctrl_bSmoothAxes ? 1 : 0;
  gm_pAccelTrigger->mg_iSelected = _pShell->GetINDEX("inp_bAllowMouseAcceleration") ? 1 : 0;
  gm_pIFeelTrigger->SetEnabled(_pShell->GetINDEX("sys_bIFeelEnabled") ? 1 : 0);
  gm_pIFeelTrigger->mg_iSelected = _pShell->GetFLOAT("inp_fIFeelGain")>0 ? 1 : 0;

  // Apply selection to triggers.
  gm_pInvertTrigger->ApplyCurrentSelection();
  gm_pSmoothTrigger->ApplyCurrentSelection();
  gm_pAccelTrigger->ApplyCurrentSelection();
  gm_pIFeelTrigger->ApplyCurrentSelection();
}

void CControlsMenu::ApplyActionSettings(void)
{
  CControls &ctrls = _pGame->gm_ctrlControlsExtra;

  FLOAT fSensitivity =
    FLOAT(gm_pSensitivity->mg_iCurPos - gm_pSensitivity->mg_iMinPos) /
    FLOAT(gm_pSensitivity->mg_iMaxPos - gm_pSensitivity->mg_iMinPos)*100.0f;

  BOOL bInvert = gm_pInvertTrigger->mg_iSelected != 0;
  BOOL bSmooth = gm_pSmoothTrigger->mg_iSelected != 0;
  BOOL bAccel = gm_pAccelTrigger->mg_iSelected != 0;
  BOOL bIFeel = gm_pIFeelTrigger->mg_iSelected != 0;

  if (INDEX(ctrls.ctrl_fSensitivity) != INDEX(fSensitivity)) {
    ctrls.ctrl_fSensitivity = fSensitivity;
  }
  ctrls.ctrl_bInvertLook = bInvert;
  ctrls.ctrl_bSmoothAxes = bSmooth;
  _pShell->SetINDEX("inp_bAllowMouseAcceleration", bAccel);
  _pShell->SetFLOAT("inp_fIFeelGain", bIFeel ? 1.0f : 0.0f);
  ctrls.CalculateInfluencesForAllAxis();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CControlsMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == gm_pButtons) {
      StartCustomizeKeyboardMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == gm_pAdvanced) {
      StartCustomizeAxisMenu();
      return TRUE;

    } else if (event.GuiEvent.Caller == gm_pPredefined) {
      StartControlsLoadMenu();
      return TRUE;
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}