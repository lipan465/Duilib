#pragma once

namespace DuiLib
{
	class CHorizontalSplitterWndUI : public CContainerUI
	{
		DECLARE_DUICONTROL(CHorizontalSplitterWndUI)
	public:
		CHorizontalSplitterWndUI(void);
		~CHorizontalSplitterWndUI(void);
		UINT GetControlFlags() const;
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
	private:
		RECT* m_SplitterRect;
		int m_SplitterCount;
		int m_SplitterIndex;
		int m_SplitterWidth;
		UINT m_uButtonState;
		POINT ptLastMouse;
	};
}

