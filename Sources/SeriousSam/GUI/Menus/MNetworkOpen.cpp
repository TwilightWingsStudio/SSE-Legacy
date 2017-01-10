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
#include "MNetworkOpen.h"

// --------------------------------------------------------------------------------------
// Intializes network open menu.
// --------------------------------------------------------------------------------------
void CNetworkOpenMenu::Initialize_t(void)
{
  // Initialize title label.
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.mg_strText = TRANS("JOIN");

  // Initialize "Address" label.
  gm_mgAddressLabel.mg_strText = TRANS("Address:");
  gm_mgAddressLabel.mg_boxOnScreen = BoxMediumLeft(1);
  gm_mgAddressLabel.mg_iCenterI = -1;

  // Initialize "Address" edit.
  gm_mgAddress.mg_strText = _pGame->gam_strJoinAddress;
  gm_mgAddress.mg_ctMaxStringLen = 20;
  gm_mgAddress.mg_pstrToChange = &_pGame->gam_strJoinAddress;
  gm_mgAddress.mg_boxOnScreen = BoxMediumMiddle(1);
  gm_mgAddress.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgAddress.mg_iCenterI = -1;
  gm_mgAddress.mg_pmgUp = &gm_mgJoin;
  gm_mgAddress.mg_pmgDown = &gm_mgPort;
  gm_mgAddress.mg_strTip = TRANS("specify server address");

  // Initialize "Port" label.
  gm_mgPortLabel.mg_strText = TRANS("Port:");
  gm_mgPortLabel.mg_boxOnScreen = BoxMediumLeft(2);
  gm_mgPortLabel.mg_iCenterI = -1;

  // Initialize "Port" edit.
  gm_mgPort.mg_strText = "";
  gm_mgPort.mg_ctMaxStringLen = 10;
  gm_mgPort.mg_pstrToChange = &gm_strPort;
  gm_mgPort.mg_boxOnScreen = BoxMediumMiddle(2);
  gm_mgPort.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPort.mg_iCenterI = -1;
  gm_mgPort.mg_pmgUp = &gm_mgAddress;
  gm_mgPort.mg_pmgDown = &gm_mgJoin;
  gm_mgPort.mg_strTip = TRANS("specify server address");

  // Initialize "Join" button.
  gm_mgJoin.mg_boxOnScreen = BoxMediumMiddle(3);
  gm_mgJoin.mg_pmgUp = &gm_mgPort;
  gm_mgJoin.mg_pmgDown = &gm_mgAddress;
  gm_mgJoin.mg_strText = TRANS("Join");
  gm_mgJoin.mg_pActivatedFunction = NULL;
  
  // Add components.
  gm_lhChildren.AddTail(gm_mgTitle.mg_lnNode);
  gm_lhChildren.AddTail(gm_mgAddressLabel.mg_lnNode);
  gm_lhChildren.AddTail(gm_mgAddress.mg_lnNode);
  gm_lhChildren.AddTail(gm_mgPortLabel.mg_lnNode);
  gm_lhChildren.AddTail(gm_mgPort.mg_lnNode);
  gm_lhChildren.AddTail(gm_mgJoin.mg_lnNode);
}

void CNetworkOpenMenu::StartMenu(void)
{
  gm_strPort = _pShell->GetValue("net_iPort");
  gm_mgPort.mg_strText = gm_strPort;
}

void CNetworkOpenMenu::EndMenu(void)
{
  _pShell->SetValue("net_iPort", gm_strPort);
}