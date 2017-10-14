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

#ifndef SE_INCL_MENU_GADGET_SLIDER_H
#define SE_INCL_MENU_GADGET_SLIDER_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "MGButton.h"

// --------------------------------------------------------------------------------------
// Class that provides horizontal slider.
// --------------------------------------------------------------------------------------
class CMGSlider : public CMGButton
{
  public:
    FLOAT mg_fFactor;
    INDEX mg_iMinPos;
    INDEX mg_iMaxPos;
    INDEX mg_iCurPos;
    
  public:
    // Default constructor. Constructs component without text.
    CMGSlider();
    
    // Constructs component with the text.
    CMGSlider(const CTString &strText);

    void ApplyCurrentPosition(void);
    void ApplyGivenPosition(INDEX iMin, INDEX iMax, INDEX iCur);
    virtual BOOL OnKeyDown(int iVKey); // return TRUE if handled
    void(*mg_pOnSliderChange)(INDEX iCurPos);
    PIXaabbox2D GetSliderBox(void);
    void Render(CDrawPort *pdp);
};

#endif  /* include-once check. */