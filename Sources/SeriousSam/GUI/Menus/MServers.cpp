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
#include "MenuManager.h"
#include "MServers.h"

CTString _strServerFilter[7];
CMGButton mgServerColumn[7];
CMGEdit mgServerFilter[7];

static void SortByColumn(int i)
{
  CServersMenu &gmCurrent = _pGUIM->gmServersMenu;

  if (gmCurrent.gm_mgList.mg_iSort == i) {
    gmCurrent.gm_mgList.mg_bSortDown = !gmCurrent.gm_mgList.mg_bSortDown;
  } else {
    gmCurrent.gm_mgList.mg_bSortDown = FALSE;
  }
  gmCurrent.gm_mgList.mg_iSort = i;
}

static void SortByServer(void) { SortByColumn(0); }
static void SortByMap(void)    { SortByColumn(1); }
static void SortByPing(void)   { SortByColumn(2); }
static void SortByPlayers(void){ SortByColumn(3); }
static void SortByGame(void)   { SortByColumn(4); }
static void SortByMod(void)    { SortByColumn(5); }
static void SortByVer(void)    { SortByColumn(6); }

void CServersMenu::Initialize_t(void)
{
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("CHOOSE SERVER");
  AddChild(&gm_mgTitle);

  gm_mgList.mg_boxOnScreen = FLOATaabbox2D(FLOAT2D(0, 0), FLOAT2D(1, 1));
  gm_mgList.mg_pmgLeft = &gm_mgList;  // make sure it can get focus
  gm_mgList.SetEnabled(TRUE);
  AddChild(&gm_mgList);

  ASSERT(ARRAYCOUNT(mgServerColumn) == ARRAYCOUNT(mgServerFilter));
  for (INDEX i = 0; i<ARRAYCOUNT(mgServerFilter); i++) {
    mgServerColumn[i].mg_strText = "";
    mgServerColumn[i].mg_boxOnScreen = BoxPlayerEdit(5.0);
    mgServerColumn[i].mg_bfsFontSize = BFS_SMALL;
    mgServerColumn[i].mg_iCenterI = -1;
    mgServerColumn[i].mg_pmgUp = &gm_mgList;
    mgServerColumn[i].mg_pmgDown = &mgServerFilter[i];
    AddChild(&mgServerColumn[i]);

    mgServerFilter[i].mg_ctMaxStringLen = 25;
    mgServerFilter[i].mg_boxOnScreen = BoxPlayerEdit(5.0);
    mgServerFilter[i].mg_bfsFontSize = BFS_SMALL;
    mgServerFilter[i].mg_iCenterI = -1;
    mgServerFilter[i].mg_pmgUp = &mgServerColumn[i];
    mgServerFilter[i].mg_pmgDown = &gm_mgList;
    AddChild(&mgServerFilter[i]);
    mgServerFilter[i].mg_pstrToChange = &_strServerFilter[i];
    mgServerFilter[i].SetText(*mgServerFilter[i].mg_pstrToChange);
  }

  gm_mgRefresh.mg_strText = TRANS("REFRESH");
  gm_mgRefresh.mg_boxOnScreen = BoxLeftColumn(15.0);
  gm_mgRefresh.mg_bfsFontSize = BFS_SMALL;
  gm_mgRefresh.mg_iCenterI = -1;
  gm_mgRefresh.mg_pmgDown = &gm_mgList;
  AddChild(&gm_mgRefresh);

  CTString astrColumns[7];
  mgServerColumn[0].mg_strText = TRANS("Server");
  mgServerColumn[1].mg_strText = TRANS("Map");
  mgServerColumn[2].mg_strText = TRANS("Ping");
  mgServerColumn[3].mg_strText = TRANS("Players");
  mgServerColumn[4].mg_strText = TRANS("Game");
  mgServerColumn[5].mg_strText = TRANS("Mod");
  mgServerColumn[6].mg_strText = TRANS("Ver");
  mgServerColumn[0].mg_strTip = TRANS("sort by server");
  mgServerColumn[1].mg_strTip = TRANS("sort by map");
  mgServerColumn[2].mg_strTip = TRANS("sort by ping");
  mgServerColumn[3].mg_strTip = TRANS("sort by players");
  mgServerColumn[4].mg_strTip = TRANS("sort by game");
  mgServerColumn[5].mg_strTip = TRANS("sort by mod");
  mgServerColumn[6].mg_strTip = TRANS("sort by version");
  mgServerFilter[0].mg_strTip = TRANS("filter by server");
  mgServerFilter[1].mg_strTip = TRANS("filter by map");
  mgServerFilter[2].mg_strTip = TRANS("filter by ping (ie. <200)");
  mgServerFilter[3].mg_strTip = TRANS("filter by players (ie. >=2)");
  mgServerFilter[4].mg_strTip = TRANS("filter by game (ie. coop)");
  mgServerFilter[5].mg_strTip = TRANS("filter by mod");
  mgServerFilter[6].mg_strTip = TRANS("filter by version");
}


static void RefreshServerList(void)
{
  _pNetwork->EnumSessions(_pGUIM->gmServersMenu.m_bInternet);
}

void CServersMenu::StartMenu(void)
{
  CGameMenu::StartMenu();
}

void CServersMenu::Think(void)
{
  if (!_pNetwork->ga_bEnumerationChange) {
    return;
  }
  _pNetwork->ga_bEnumerationChange = FALSE;
}

// [SSE]
BOOL CServersMenu::OnEvent(const SEvent& event)
{
  if (event.EventType == EET_GUI_EVENT)
  {
    if (event.GuiEvent.EventType == EGET_TRIGGERED)
    {
      if (event.GuiEvent.Caller == &gm_mgRefresh) {
        RefreshServerList();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[0] || event.GuiEvent.Caller == &mgServerColumn[0]) {
        SortByServer();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[1] || event.GuiEvent.Caller == &mgServerColumn[1]) {
        SortByMap();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[2] || event.GuiEvent.Caller == &mgServerColumn[2]) {
        SortByPing();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[3] || event.GuiEvent.Caller == &mgServerColumn[3]) {
        SortByPlayers();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[4] || event.GuiEvent.Caller == &mgServerColumn[4]) {
        SortByGame();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[5] || event.GuiEvent.Caller == &mgServerColumn[5]) {
        SortByMod();
        return TRUE;
      
      } else if (event.GuiEvent.Caller == &mgServerFilter[6] || event.GuiEvent.Caller == &mgServerColumn[6]) {
        SortByVer();
        return TRUE;
      }
    }
  }
  
  if (CGameMenu::OnEvent(event)) {
    return TRUE;
  }
  
  return m_pParent ? m_pParent->OnEvent(event) : FALSE;
}