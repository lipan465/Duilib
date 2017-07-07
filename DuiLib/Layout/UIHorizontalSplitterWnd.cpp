#include "StdAfx.h"
#include "UIHorizontalSplitterWnd.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CHorizontalSplitterWndUI)
	CHorizontalSplitterWndUI::CHorizontalSplitterWndUI(void)
	{
		m_uButtonState = 0;
		m_SplitterCount = 0;
		m_SplitterIndex = 0;
		m_SplitterWidth = 4;
		m_SplitterRect = NULL;
	}

	CHorizontalSplitterWndUI::~CHorizontalSplitterWndUI(void)
	{
		if(m_SplitterRect) delete m_SplitterRect;
	}

	void CHorizontalSplitterWndUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		rc = m_rcItem;

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		int cyNeeded = 0;
		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		SIZE szControlAvailable;
		int iControlMaxWidth = 0;
		int iControlMaxHeight = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			szControlAvailable = szAvailable;
			RECT rcPadding = pControl->GetPadding();
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cx == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;

			sz.cy = MAX(sz.cy, 0);
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			cyNeeded = MAX(cyNeeded, sz.cy + rcPadding.top + rcPadding.bottom);
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		if( m_SplitterCount != (nEstimateNum - 1) )
		{
			m_SplitterCount = nEstimateNum - 1;
			if(m_SplitterRect) delete m_SplitterRect;
			if(m_SplitterCount!= 0) m_SplitterRect = new RECT[m_SplitterCount];
			else m_SplitterRect = NULL;
		}
		// Place elements
		int cxNeeded = 0;
		int cxExpand = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		int iEstimate = 0;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			iEstimate += 1;
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			szControlAvailable = szRemaining;
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
			cxFixedRemaining = cxFixedRemaining - (rcPadding.left + rcPadding.right);
			if (iEstimate > 1) cxFixedRemaining = cxFixedRemaining - m_iChildPadding;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				} 
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}

			if(pControl->GetFixedHeight() <= 0) {
				sz.cy = pControl->GetMaxHeight();
				if( sz.cy == 0 ) sz.cy = szAvailable.cy - rcPadding.top - rcPadding.bottom;
				if( sz.cy < 0 ) sz.cy = 0;
				if( sz.cy > szControlAvailable.cy ) sz.cy = szControlAvailable.cy;
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			}

			{
				int iPosY = rc.top;
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy + rcPadding.top };
				if(it2 < m_SplitterCount )
				{
					rcCtrl.right -= m_SplitterWidth;
					m_SplitterRect[it2].left = rcCtrl.right;
					m_SplitterRect[it2].right = rcCtrl.right + m_SplitterWidth;
					m_SplitterRect[it2].top = rcCtrl.top;
					m_SplitterRect[it2].bottom = rcCtrl.bottom;
				}
				pControl->SetPos(rcCtrl, false);
			}

			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
	}

	UINT CHorizontalSplitterWndUI::GetControlFlags() const
	{
		if( IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CHorizontalSplitterWndUI::DoEvent(TEventUI& event)
	{
		if( m_SplitterCount > 0 )
		{
			if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
			{
				for(int i=0;i<m_SplitterCount;i++)
				{
					RECT rcSeparator = m_SplitterRect[i];
					if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
						m_uButtonState |= UISTATE_CAPTURED;
						ptLastMouse = event.ptMouse;
						m_SplitterIndex = i;
						return;
					}
				}
			}
			if( event.Type == UIEVENT_BUTTONUP )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					m_uButtonState &= ~UISTATE_CAPTURED;
					return;
				}
			}
			if( event.Type == UIEVENT_MOUSEMOVE )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					LONG cx = event.ptMouse.x - ptLastMouse.x;
					CDuiRect rect = GetPos();
					if( (rect.right - event.ptMouse.x) <= m_SplitterWidth) return;
					if( (event.ptMouse.x - rect.left) <= m_SplitterWidth ) return;
					ptLastMouse = event.ptMouse;
					CControlUI* pControl1 = static_cast<CControlUI*>(m_items[m_SplitterIndex]);
					RECT rc1 = pControl1->GetPos();
					rc1.right += cx;
					pControl1->SetPos(rc1);

					m_SplitterRect[m_SplitterIndex].right += cx;
					m_SplitterRect[m_SplitterIndex].left += cx;

					CControlUI* pControl2 = static_cast<CControlUI*>(m_items[m_SplitterIndex+1]);
					RECT rc2 = pControl2->GetPos();
					rc2.left += cx;
					pControl2->SetPos(rc2);
				}
			}
			if( event.Type == UIEVENT_SETCURSOR )
			{
				for(int i=0;i<m_SplitterCount;i++)
				{
					RECT rcSeparator = m_SplitterRect[i];
					if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
						::SetCursor(::LoadCursor(ghModule, MAKEINTRESOURCE(IDC_CURSOR_SIZEWE)));
						return;
					}
				}
			}
		}
		CContainerUI::DoEvent(event);
	}
}
