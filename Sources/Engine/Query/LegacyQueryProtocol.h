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


#ifndef SE_INCL_LEGACY_QUERY_PROTOCOL_H
#define SE_INCL_LEGACY_QUERY_PROTOCOL_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/API/IQueryProtocol.h>

class CLegacyQueryProtocol : public IQueryProtocol
{
  public:
    void BuildHearthbeatPacket(CTString &strPacket);
  
  public:
    virtual void EnumTrigger(BOOL bInternet);
    virtual void EnumUpdate(void);
    virtual void ClientParsePacket(INDEX iLength) {};
    virtual void ServerParsePacket(INDEX iLength);
};

#endif  /* include-once check. */