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

#include "MGButton.h"
#include "MGCheckBox.h"
#include <GameMP/LCDDrawing.h>


// --------------------------------------------------------------------------------------
// Default constructor. Constructs component without text.
// --------------------------------------------------------------------------------------
CMGCheckBox::CMGCheckBox(void)
{
  mg_pActivatedFunction = NULL;
  
  mg_bValue = FALSE;
  
  mg_iIndex = 0;
  mg_iCenterI = 0;
  mg_iTextMode = 1;
  mg_bfsFontSize = BFS_MEDIUM;
  mg_iCursorPos = -1;
  mg_bRectangle = FALSE;
  mg_bBlinking = FALSE;
  mg_bEditing = FALSE;
  mg_bHighlighted = FALSE;
}

// --------------------------------------------------------------------------------------
// Called every time when user pressed a button while this component active.
// --------------------------------------------------------------------------------------
BOOL CMGCheckBox::OnKeyDown(int iVKey)
{
  if ((iVKey == VK_RETURN || iVKey == VK_LBUTTON) ||
    (iVKey == VK_LEFT) ||
    (iVKey == VK_BACK || iVKey == VK_RBUTTON) ||
    (iVKey == VK_RIGHT))
  {
    // key is handled
    if (mg_bEnabled)
    {
      mg_bValue = !mg_bValue;

      // [SSE]
      if (m_pParent != NULL)
      {
        SEvent newEvent;
        newEvent.EventType = EET_GUI_EVENT;
        newEvent.GuiEvent.Caller = this;
        newEvent.GuiEvent.Target = NULL;
        newEvent.GuiEvent.EventType = EGET_CHANGED;
        newEvent.GuiEvent.IntValue = mg_bValue;
        
        m_pParent->OnEvent(newEvent);
      }
    }

    return TRUE;
  }

  // key is not handled
  return FALSE;
}

// --------------------------------------------------------------------------------------
// Draws component.
// --------------------------------------------------------------------------------------
void CMGCheckBox::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);
  
  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1) * 0.45f;
  PIX pixIR = box.Min()(1) + box.Size()(1) * 0.55f;
  PIX pixJ = box.Min()(2);

  COLOR col = GetCurrentColor();
  
  COLOR colBorder = LCDGetColor(C_GREEN | 255, "slider box");

  //pdp->Fill(box.Min()(1), box.Min()(2), box.Size()(1), box.Size()(2), C_GREEN|CT_OPAQUE);

  if (mg_iCenterI == -1) {
    pdp->PutText(mg_strText, box.Min()(1), pixJ, col);
    //pdp->PutTextR(strValue, box.Max()(1), pixJ, col);
  } else {
    pdp->PutTextR(mg_strText, pixIL, pixJ, col);
    
    static CTFileName fnmYes = CTFILENAME("Textures\\Menu\\CheckBoxYes.tex");;
    static CTFileName fnmNo = CTFILENAME("Textures\\Menu\\CheckBoxNo.tex");;
    
    CTextureObject to;

    try {
      if (mg_bValue) {
        to.SetData_t(fnmYes);
      } else {
        to.SetData_t(fnmNo);
      }

      CTextureData *ptd = (CTextureData *)to.GetData();
      PIX pixSize = pdp->dp_FontData->GetHeight() * pdp->dp_fTextScaling * 0.8F;

      pdp->PutTexture(&to, PIXaabbox2D(
        PIX2D(pixIR, pixJ),
        PIX2D(pixIR + pixSize, pixJ + pixSize)), col | 255);
    } catch (char *strError) {
      CPrintF("%s\n", strError);
    }

    to.SetData(NULL);
  }
}