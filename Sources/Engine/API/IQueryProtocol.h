/* Copyright (c) 2018 by ZCaliptium.

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


#ifndef SE_INCL_IQUERYPROTOCOL_H
#define SE_INCL_IQUERYPROTOCOL_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

class IQueryProtocol
{
  public:
    virtual void EnumTrigger(BOOL bInternet) = 0;
    virtual void EnumUpdate(void) = 0;
    virtual void ClientParsePacket(INDEX iLength) = 0;
    virtual void ServerParsePacket(INDEX iLength) = 0;
};

#endif  /* include-once check. */