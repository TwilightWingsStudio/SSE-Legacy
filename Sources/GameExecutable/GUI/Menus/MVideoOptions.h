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

#ifndef SE_INCL_GAME_MENU_VIDEOOPTIONS_H
#define SE_INCL_GAME_MENU_VIDEOOPTIONS_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include "GameMenu.h"
#include "GUI/Components/MGCheckBox.h"

class CVideoOptionsMenu : public CGameMenu
{
  public:
    CMGTitle *gm_pTitle;
    CMGTrigger *gm_pDisplayAPITrigger;
    CMGTrigger *gm_pDisplayAdaptersTrigger;
    CMGCheckBox *gm_pFullScreenCheckBox;
    CMGTrigger *gm_pAspectRatioTrigger;
    CMGTrigger *gm_pResolutionsTrigger;
    CMGTrigger *gm_pDisplayPrefsTrigger;
    CMGButton *gm_pVideoRendering;
    CMGTrigger *gm_pBitsPerPixelTrigger;
    CMGButton *gm_pApply;

  public:
    void StartMenu(void);
    void Initialize_t(void);
    virtual BOOL OnEvent(const SEvent& event); // [SSE]
};

#endif  /* include-once check. */