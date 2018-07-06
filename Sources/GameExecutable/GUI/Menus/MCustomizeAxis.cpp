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
#include "MCustomizeAxis.h"

// --------------------------------------------------------------------------------------
// Intializes axis menu.
// --------------------------------------------------------------------------------------
void CCustomizeAxisMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_pTitle = new CMGTitle(TRANS("CUSTOMIZE AXIS"));
  gm_pTitle->mg_boxOnScreen = BoxTitle();

  // Initialize "Action" trigger.
  gm_pActionTrigger = new CMGTrigger(TRANS("ACTION"));
  gm_pActionTrigger->mg_strTip = TRANS("choose action to customize");
  gm_pActionTrigger->mg_boxOnScreen = BoxMediumRow(0);
  gm_pActionTrigger->mg_astrTexts = new CTString[AXIS_ACTIONS_CT];
  gm_pActionTrigger->mg_ctTexts = AXIS_ACTIONS_CT;

  // For all available axis type controlers.
  for (INDEX iControler = 0; iControler<AXIS_ACTIONS_CT; iControler++) {
    gm_pActionTrigger->mg_astrTexts[iControler] = TranslateConst(CTString(_pGame->gm_astrAxisNames[iControler]), 0);
  }

  gm_pActionTrigger->mg_iSelected = 3;

  // Initialize "Mounted to" trigger.
  gm_pMountedTrigger = new CMGTrigger(TRANS("MOUNTED TO"));
  gm_pMountedTrigger->mg_strTip = TRANS("choose controller axis that will perform the action");
  gm_pMountedTrigger->mg_boxOnScreen = BoxMediumRow(1);
  gm_pMountedTrigger->mg_astrTexts = astrNoYes;
  gm_pMountedTrigger->mg_ctTexts = 2;

  INDEX ctAxis = _pInput->GetAvailableAxisCount();
  gm_pMountedTrigger->mg_astrTexts = new CTString[ctAxis];
  gm_pMountedTrigger->mg_ctTexts = ctAxis;

  // For all axis actions that can be mounted.
  for (INDEX iAxis = 0; iAxis<ctAxis; iAxis++) {
    gm_pMountedTrigger->mg_astrTexts[iAxis] = _pInput->GetAxisTransName(iAxis);
  }

  // Initialize "Sensitivity" slider.
  gm_pSensitivity = new CMGSlider(TRANS("SENSITIVITY"));
  gm_pSensitivity->mg_strTip = TRANS("set sensitivity for this axis");
  gm_pSensitivity->mg_boxOnScreen = BoxMediumRow(3);
  gm_pSensitivity->mg_iMinPos = 0;
  gm_pSensitivity->mg_iMaxPos = 50;

  // Initialize "Dead Zone" slider.
  gm_pDeadzone = new CMGSlider(TRANS("DEAD ZONE"));
  gm_pDeadzone->mg_strTip = TRANS("set dead zone for this axis");
  gm_pDeadzone->mg_boxOnScreen = BoxMediumRow(4);
  gm_pDeadzone->mg_iMinPos = 0;
  gm_pDeadzone->mg_iMaxPos = 50;
  
  // Initialize "Inverted" trigger.
  gm_pInvertTrigger = new CMGTrigger(TRANS("INVERTED"));
  gm_pInvertTrigger->mg_strTip = TRANS("choose whether to invert this axis or not");
  gm_pInvertTrigger->mg_boxOnScreen = BoxMediumRow(5);
  gm_pInvertTrigger->mg_astrTexts = astrNoYes;
  gm_pInvertTrigger->mg_ctTexts = 2;

  // Initialize "Relative" trigger.
  gm_pRelativeTrigger = new CMGTrigger(TRANS("RELATIVE"));
  gm_pRelativeTrigger->mg_strTip = TRANS("select relative or absolute axis reading");
  gm_pRelativeTrigger->mg_boxOnScreen = BoxMediumRow(6);
  gm_pRelativeTrigger->mg_astrTexts = astrNoYes;
  gm_pRelativeTrigger->mg_ctTexts = 2;

  // Initialize "Smooth" trigger.
  gm_pSmoothTrigger = new CMGTrigger(TRANS("SMOOTH"));
  gm_pSmoothTrigger->mg_strTip = TRANS("turn this on to filter readings on this axis");
  gm_pSmoothTrigger->mg_boxOnScreen = BoxMediumRow(7);
  gm_pSmoothTrigger->mg_astrTexts = astrNoYes;
  gm_pSmoothTrigger->mg_ctTexts = 2;

  // Define neighbours.
  gm_pActionTrigger->mg_pmgUp = gm_pSmoothTrigger;
  gm_pActionTrigger->mg_pmgDown = gm_pMountedTrigger;
  gm_pMountedTrigger->mg_pmgUp = gm_pActionTrigger;
  gm_pMountedTrigger->mg_pmgDown = gm_pSensitivity;
  gm_pSensitivity->mg_pmgUp = gm_pMountedTrigger;
  gm_pSensitivity->mg_pmgDown = gm_pDeadzone;
  gm_pDeadzone->mg_pmgUp = gm_pSensitivity;
  gm_pDeadzone->mg_pmgDown = gm_pInvertTrigger;
  gm_pInvertTrigger->mg_pmgUp = gm_pDeadzone;
  gm_pInvertTrigger->mg_pmgDown = gm_pRelativeTrigger;
  gm_pRelativeTrigger->mg_pmgUp = gm_pInvertTrigger;
  gm_pRelativeTrigger->mg_pmgDown = gm_pSmoothTrigger;
  gm_pSmoothTrigger->mg_pmgUp = gm_pRelativeTrigger;
  gm_pSmoothTrigger->mg_pmgDown = gm_pActionTrigger;
  
  // Add components.
  AddChild(gm_pTitle);
  AddChild(gm_pActionTrigger);
  AddChild(gm_pMountedTrigger);
  AddChild(gm_pSensitivity);
  AddChild(gm_pDeadzone);
  AddChild(gm_pInvertTrigger);
  AddChild(gm_pRelativeTrigger);
  AddChild(gm_pSmoothTrigger);
}

CCustomizeAxisMenu::~CCustomizeAxisMenu(void)
{
  delete[] gm_pActionTrigger->mg_astrTexts;
  delete[] gm_pMountedTrigger->mg_astrTexts;
}

void CCustomizeAxisMenu::ObtainActionSettings(void)
{
  ControlsMenuOn();
  CControls &ctrls = _pGame->gm_ctrlControlsExtra;
  INDEX iSelectedAction = gm_pActionTrigger->mg_iSelected;
  INDEX iMountedAxis = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction;

  gm_pMountedTrigger->mg_iSelected = iMountedAxis;

  gm_pSensitivity->mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity / 2;
  gm_pSensitivity->ApplyCurrentPosition();

  gm_pDeadzone->mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone / 2;
  gm_pDeadzone->ApplyCurrentPosition();

  gm_pInvertTrigger->mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert ? 1 : 0;
  gm_pRelativeTrigger->mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler ? 1 : 0;
  gm_pSmoothTrigger->mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth ? 1 : 0;

  gm_pActionTrigger->ApplyCurrentSelection();
  gm_pMountedTrigger->ApplyCurrentSelection();
  gm_pInvertTrigger->ApplyCurrentSelection();
  gm_pRelativeTrigger->ApplyCurrentSelection();
  gm_pSmoothTrigger->ApplyCurrentSelection();
}

void CCustomizeAxisMenu::ApplyActionSettings(void)
{
  CControls &ctrls = _pGame->gm_ctrlControlsExtra;

  INDEX iSelectedAction = gm_pActionTrigger->mg_iSelected;
  INDEX iMountedAxis = gm_pMountedTrigger->mg_iSelected;
  FLOAT fSensitivity =
    FLOAT(gm_pSensitivity->mg_iCurPos - gm_pSensitivity->mg_iMinPos) /
    FLOAT(gm_pSensitivity->mg_iMaxPos - gm_pSensitivity->mg_iMinPos)*100.0f;
  FLOAT fDeadZone =
    FLOAT(gm_pDeadzone->mg_iCurPos - gm_pDeadzone->mg_iMinPos) /
    FLOAT(gm_pDeadzone->mg_iMaxPos - gm_pDeadzone->mg_iMinPos)*100.0f;

  BOOL bInvert = gm_pInvertTrigger->mg_iSelected != 0;
  BOOL bRelative = gm_pRelativeTrigger->mg_iSelected != 0;
  BOOL bSmooth = gm_pSmoothTrigger->mg_iSelected != 0;

  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction = iMountedAxis;
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity) != INDEX(fSensitivity)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity = fSensitivity;
  }
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone) != INDEX(fDeadZone)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone = fDeadZone;
  }
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert = bInvert;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler = bRelative;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth = bSmooth;
  ctrls.CalculateInfluencesForAllAxis();

  ControlsMenuOff();
}

void CCustomizeAxisMenu::StartMenu(void)
{
  ObtainActionSettings();

  CGameMenu::StartMenu();
}

void CCustomizeAxisMenu::EndMenu(void)
{
  ApplyActionSettings();

  CGameMenu::EndMenu();
}

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CCustomizeAxisMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_CHANGED) {
      if (event.GuiEvent.Caller == gm_pActionTrigger) {
        ObtainActionSettings();
      }

      return TRUE;

    } else if (event.GuiEvent.EventType == EGET_PRECHANGE) {
      if (event.GuiEvent.Caller == gm_pActionTrigger) {
        ApplyActionSettings();
      }
      return TRUE;
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}