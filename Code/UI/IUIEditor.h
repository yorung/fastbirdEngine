#pragma once
#include <UI/UIProperty.h>
#include <CommonLib/IteratorWrapper.h>
namespace fastbird
{
	class IWinBase;
	class IUIEditor
	{
	public:
		typedef std::vector<IWinBase*> Comps;
		virtual void OnComponentSelected(IWinBase* comp) {}
		virtual void OnComponentDeselected(IWinBase* comp) {}
		virtual void OnComponentDeleted(IWinBase* comp){}
		virtual IWinBase* GetCurSelected() const { return 0; }
		virtual unsigned GetNumCurEditing() const { return 0; }
		virtual IWinBase* GetCurSelected(unsigned index) const { return 0; }
		virtual IteratorWrapper<Comps> GetSelectedComps() = 0;
		virtual void OnCancelComponent() {}
		virtual void OnPosSizeChanged(){}
		virtual void DrawFocusBoxes(){}
		virtual void TryToDeleteCurComp() {}
		virtual HWND_ID GetHwndId() const { return -1; }
		virtual void BackupSizePos(){}
		virtual void RestoreSizePos(){}

		// process for Ctrl + left, right, up, down. and Ctrl+keypad 5
		virtual void ProcessKeyInput(){}

		virtual void Save(){}
		
	};
}