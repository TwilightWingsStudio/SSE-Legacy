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
#include "MConfirm.h"

// --------------------------------------------------------------------------------------
// Intializes confirmation menu.
// --------------------------------------------------------------------------------------
void CConfirmMenu::Initialize_t(void)
{
  gm_bPopup = TRUE;

  // Initialize title label.
  gm_pConfirmLabel = new CMGButton();
  gm_pConfirmLabel->mg_boxOnScreen = BoxPopupLabel();
  gm_pConfirmLabel->mg_iCenterI = 0;
  gm_pConfirmLabel->mg_bfsFontSize = BFS_LARGE;

  // Initialize "Yes" button.
  gm_pConfirmYes = new CMGButton(TRANS("YES"));
  gm_pConfirmYes->mg_boxOnScreen = BoxPopupYesLarge();
  gm_pConfirmYes->mg_pmgLeft = gm_pConfirmYes->mg_pmgRight = gm_pConfirmNo;
  gm_pConfirmYes->mg_iCenterI = 1;
  gm_pConfirmYes->mg_bfsFontSize = BFS_LARGE;

  // Initialize "No" button.
  gm_pConfirmNo = new CMGButton(TRANS("NO"));
  gm_pConfirmNo->mg_boxOnScreen = BoxPopupNoLarge();
  gm_pConfirmNo->mg_pmgLeft = gm_pConfirmNo->mg_pmgRight = gm_pConfirmYes;
  gm_pConfirmNo->mg_iCenterI = -1;
  gm_pConfirmNo->mg_bfsFontSize = BFS_LARGE;
  
  // Add components.  
  AddChild(gm_pConfirmLabel);
  AddChild(gm_pConfirmYes);
  AddChild(gm_pConfirmNo);

  _pConfimedYes = NULL;
  _pConfimedNo = NULL;
}

void CConfirmMenu::BeLarge(void)
{
  gm_pConfirmLabel->mg_bfsFontSize = BFS_LARGE;
  gm_pConfirmYes->mg_bfsFontSize = BFS_LARGE;
  gm_pConfirmNo->mg_bfsFontSize = BFS_LARGE;

  gm_pConfirmLabel->mg_iCenterI = 0;
  gm_pConfirmYes->mg_boxOnScreen = BoxPopupYesLarge();
  gm_pConfirmNo->mg_boxOnScreen = BoxPopupNoLarge();
}

void CConfirmMenu::BeSmall(void)
{
  gm_pConfirmLabel->mg_bfsFontSize = BFS_MEDIUM;
  gm_pConfirmYes->mg_bfsFontSize = BFS_MEDIUM;
  gm_pConfirmNo->mg_bfsFontSize = BFS_MEDIUM;

  gm_pConfirmLabel->mg_iCenterI = -1;
  gm_pConfirmYes->mg_boxOnScreen = BoxPopupYesSmall();
  gm_pConfirmNo->mg_boxOnScreen = BoxPopupNoSmall();
}

// return TRUE if handled
BOOL CConfirmMenu::OnKeyDown(int iVKey)
{
  if ((iVKey == VK_ESCAPE || iVKey == VK_RBUTTON) && gm_pConfirmNo->GetParent() != NULL) {
    gm_pConfirmNo->OnActivate();
    return TRUE;
  }

  return CGameMenu::OnKeyDown(iVKey);
}

void MenuGoToParent(void);

// --------------------------------------------------------------------------------------
// [SSE]
// Returns TRUE if event was handled.
// --------------------------------------------------------------------------------------
BOOL CConfirmMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.Caller == gm_pConfirmYes) {
      if (_pConfimedYes != NULL) {
        _pConfimedYes();
      }

      void MenuGoToParent(void);
      MenuGoToParent();
      return TRUE;

    } else if (event.GuiEvent.Caller == gm_pConfirmNo) {
      if (_pConfimedNo != NULL) {
        _pConfimedNo();
      }

      MenuGoToParent();
      return TRUE;
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}