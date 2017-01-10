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
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include <GameMP/LCDDrawing.h>
#include "LevelInfo.h"
#include "VarList.h"
#include "MenuGadget.h"

extern CSoundData *_psdSelect;

extern BOOL _bDefiningKey = FALSE;
extern BOOL _bEditingString = FALSE;
extern CMenuGadget *_pmgLastActivatedGadget = NULL;

// --------------------------------------------------------------------------------------
// Constructor.
// --------------------------------------------------------------------------------------
CMenuGadget::CMenuGadget(void)
{
  mg_pmgLeft = NULL;
  mg_pmgRight = NULL;
  mg_pmgUp = NULL;
  mg_pmgDown = NULL;

  mg_bVisible = TRUE;
  mg_bEnabled = TRUE;
  mg_bLabel = FALSE;
  mg_bFocused = FALSE;
  mg_iInList = -1;    // not in list
}

void CMenuGadget::OnActivate(void)
{
  NOTHING;
}

// --------------------------------------------------------------------------------------
// Called every time when user pressed a button while this component active.
// --------------------------------------------------------------------------------------
BOOL CMenuGadget::OnKeyDown(int iVKey)
{
  // If return pressed...
  if (iVKey == VK_RETURN || iVKey == VK_LBUTTON) {
    OnActivate(); // Activate.
    return TRUE; // Notice that is handled.
  }

  return FALSE; // Notice that is not handled.
}


BOOL CMenuGadget::OnChar(MSG msg)
{
  return FALSE; // Notice that is not handled.
}

void CMenuGadget::OnSetFocus(void)
{
  mg_bFocused = TRUE;
  if (!IsSeparator())
  {
    PlayMenuSound(_psdSelect);
    IFeel_PlayEffect("Menu_select");
  }
}

void CMenuGadget::OnKillFocus(void)
{
  mg_bFocused = FALSE;
}

void CMenuGadget::Appear(void)
{
  mg_bVisible = TRUE;
}

void CMenuGadget::Disappear(void)
{
  mg_bVisible = FALSE;
  mg_bFocused = FALSE;
}

void CMenuGadget::Think(void)
{
}

void CMenuGadget::OnMouseOver(PIX pixI, PIX pixJ)
{
}

// get current color for the gadget
COLOR CMenuGadget::GetCurrentColor(void)
{
  // use normal colors
  COLOR colUnselected = LCDGetColor(C_GREEN, "unselected");
  COLOR colSelected = LCDGetColor(C_WHITE, "selected");
  // if disabled
  if (!mg_bEnabled) {
    // use a bit darker colors
    colUnselected = LCDGetColor(C_dGREEN, "disabled unselected");
    colSelected = LCDGetColor(C_GRAY, "disabled selected");
    // if label
    if (mg_bLabel) {
      // use white
      colUnselected = colSelected = LCDGetColor(C_WHITE, "label");
    }
  }
 
  COLOR colRet = colUnselected; // Use unselected color.

  // If selected then oscilate towards selected color.
  if (mg_bFocused) {
    FLOAT tmNow = _pTimer->GetHighPrecisionTimer().GetSeconds();
    colRet = LerpColor((colUnselected >> 1) & 0x7F7F7F7F, colSelected, sin(tmNow*10.0f)*0.5f + 0.5f);
  }

  return colRet | CT_OPAQUE;
}

// --------------------------------------------------------------------------------------
// Draws component.
// --------------------------------------------------------------------------------------
void CMenuGadget::Render(CDrawPort *pdp) {}
