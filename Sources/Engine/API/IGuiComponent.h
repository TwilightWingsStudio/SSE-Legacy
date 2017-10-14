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

#include <Engine/Base/Lists.h>
#include <Engine/API/IEventListener.h>

// --------------------------------------------------------------------------------------
// GUI Component Interface
// --------------------------------------------------------------------------------------
class IGuiComponent : public IEventListener
{
  public:
    // Constructor.
    IGuiComponent(void)
    {
      m_pParent = NULL;
      m_ulID = 0;
      m_bEnabled = TRUE;
      m_bVisible = TRUE;
    }
  
    virtual BOOL OnEvent(const SEvent& event)
    {
      return m_pParent ? m_pParent->OnEvent(event) : false;
    }
    
    void SetParent(IGuiComponent* pParent)
    {
      m_pParent = pParent;
    }
    
    // Returns component ID.
    ULONG GetID(void)
    {
      return m_ulID;
    }
    
    // Changes ID of this component.
    void SetID(ULONG ulID)
    {
      m_ulID = ulID;
    }
    
    // Returns TRUE if this component visible.
    virtual BOOL IsVisible() const 
    {
      return m_bVisible;
    }
    
    // Enables/Disables visibility of this component.
    virtual void SetVisible(BOOL bVisible)
    {
      m_bVisible = bVisible;
    }
    
    // Returns TRUE if this component enabled.
    virtual BOOL IsEnabled() const 
    {
      return m_bEnabled;
    }
    
    // Enables/Disables this component.
    virtual void SetEnabled(BOOL bEnabled)
    {
      m_bEnabled = bEnabled;
    }
    
    // Returns parent of this component.
    IGuiComponent* GetParent() const
    {
      return m_pParent;
    }
    
  public:
    CListNode m_lnNode; // for linking in list of avaliable components

  protected:
    IGuiComponent *m_pParent;
    ULONG m_ulID;
    
    BOOL m_bEnabled;
    BOOL m_bVisible;
};

#endif