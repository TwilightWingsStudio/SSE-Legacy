/* Copyright (c) 2017 by ZCaliptium

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

#ifndef SE_INCL_IGUICOMPONENT_H
#define SE_INCL_IGUICOMPONENT_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/API/IEventListener.h>

// GUI Component Interface
class IGuiComponent : public IEventListener
{
  public:
    // Constructor.
    IGuiComponent(void)
    {
      m_pParent = NULL;
      m_ulID = 0;
    }
  
    virtual BOOL OnEvent(const SEvent& event)
    {
      return m_pParent ? m_pParent->OnEvent(event) : false;
    }
    
    void SetParent(IGuiComponent* pParent)
    {
      m_pParent = pParent;
    }
    
    // Returns element ID.
    ULONG GetID(void)
    {
      return m_ulID;
    }
    
    void SetID(ULONG ulID)
    {
      m_ulID = ulID;
    }
    
    // Returns parent of this component.
    IGuiComponent* GetParent() const
    {
      return m_pParent;
    }
    
  protected:
    IGuiComponent *m_pParent;
    ULONG m_ulID;
};

#endif