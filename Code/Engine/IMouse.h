#pragma once
#ifndef _fastbird_IMouse_header_included_
#define _fastbird_IMouse_header_included_

#include <Engine/IInputDevice.h>
#include <CommonLib/Math/Vec2.h>

namespace fastbird
{
	struct MouseEvent
	{
		unsigned short usFlags;
		union {
			unsigned long ulButtons;
			struct  {
				unsigned short  usButtonFlags;
				unsigned short  usButtonData;
			};
		};
		unsigned long ulRawButtons;
		long lLastX;
		long lLastY;
		unsigned long ulExtraInformation;
	};

	class IMouse : public IInputDevice
	{
	public:
		enum MOUSE_BUTTON_FLAG
		{
			MOUSE_BUTTON_FLAG_LEFT_BUTTON_DOWN=0x0001,
			MOUSE_BUTTON_FLAG_LEFT_BUTTON_UP=0x0002,
			MOUSE_BUTTON_FLAG_RIGHT_BUTTON_DOWN=0x0004,
			MOUSE_BUTTON_FLAG_RIGHT_BUTTON_UP=0x0008,
			MOUSE_BUTTON_FLAG_MIDDLE_BUTTON_DOWN=0x0010,
			MOUSE_BUTTON_FLAG_MIDDLE_BUTTON_UP=0x0020,
			
			MOUSE_BUTTON_FLAG_BUTTON_1_DOWN=MOUSE_BUTTON_FLAG_LEFT_BUTTON_DOWN,
			MOUSE_BUTTON_FLAG_BUTTON_1_UP=MOUSE_BUTTON_FLAG_LEFT_BUTTON_UP,
			MOUSE_BUTTON_FLAG_BUTTON_2_DOWN=MOUSE_BUTTON_FLAG_RIGHT_BUTTON_DOWN,
			MOUSE_BUTTON_FLAG_BUTTON_2_UP=MOUSE_BUTTON_FLAG_RIGHT_BUTTON_UP,
			MOUSE_BUTTON_FLAG_BUTTON_3_DOWN=MOUSE_BUTTON_FLAG_MIDDLE_BUTTON_DOWN,
			MOUSE_BUTTON_FLAG_BUTTON_3_UP=MOUSE_BUTTON_FLAG_MIDDLE_BUTTON_UP,
			
			MOUSE_BUTTON_FLAG_BUTTON_4_DOWN=0x0040,
			MOUSE_BUTTON_FLAG_BUTTON_4_UP=0x0080,
			MOUSE_BUTTON_FLAG_BUTTON_5_DOWN=0x0100,
			MOUSE_BUTTON_FLAG_BUTTON_5_UP=0x0200,
			/*
			 * If usButtonFlags has RI_MOUSE_WHEEL, the wheel delta is stored in usButtonData.
			 * Take it as a signed value.
			 */
			MOUSE_BUTTON_FLAG_MOUSE_WHEEL=0x0400,
		};

		enum MOUSE_BUTTON
		{
			MOUSE_BUTTON_LEFT = 0x1,
			MOUSE_BUTTON_RIGHT = 0x2,
			MOUSE_BUTTON_MIDDLE = 0x4,
			MOUSE_BUTTON_4 = 0x8,
			MOUSE_BUTTON_5 = 0x10,
		};

		virtual void PushEvent(HWND handle, const MouseEvent& mouseEvent) = 0;
		virtual void GetHDDeltaXY(long &x, long &y) const = 0;
		virtual void GetDeltaXY(long &x, long &y) const = 0;
		virtual Vec2I GetDeltaXY() const = 0;
		virtual void GetPos(long &x, long &y) const = 0;
		virtual Vec2I GetPos() const = 0;
		virtual void GetPrevPos(long &x, long &y) const = 0;
		// normalized pos(0.0~1.0)
		virtual void GetNPos(float &x, float &y) const = 0;
		virtual Vec2 GetNPos() const = 0;
		virtual bool IsLButtonDownPrev() const = 0;
		virtual bool IsLButtonDown(float* time = 0) const = 0;
		virtual bool IsLButtonClicked() const = 0;
		virtual bool IsLButtonDoubleClicked() const = 0;
		virtual bool IsLButtonPressed() const = 0;
		virtual bool IsRButtonDown(float* time = 0) const = 0;		
		virtual bool IsRButtonDownPrev() const = 0;
		virtual bool IsRButtonClicked() const = 0;
		virtual bool IsRButtonPressed() const = 0;
		virtual bool IsMButtonDown() const = 0;
		virtual bool IsMoved() const = 0;
		virtual void GetDragStart(long &x, long &y) const = 0;
		virtual Vec2I GetDragStartedPos() const = 0;
		virtual bool IsDragStartIn(const RECT& region) const = 0;
		virtual bool IsDragStarted(Vec2I& outStartPos) const = 0;
		virtual bool IsDragEnded() const = 0;
		virtual void PopDragEvent() = 0;

		virtual bool IsRDragStarted(Vec2I& outStartPos) const = 0;
		virtual bool IsRDragEnded(Vec2I& outStartPos) const = 0;
		virtual void PopRDragEvent() = 0;
		

		virtual long GetWheel() const = 0;
		virtual void PopWheel() = 0;
		virtual void ClearWheel() = 0;
		virtual void ClearButton() = 0;
		virtual unsigned long GetNumLinesWheelScroll() const = 0;

		virtual void LockMousePos(bool lock, void* key) = 0;
		virtual void NoClickOnce() = 0;
		virtual void OnKillFocus() = 0;
		virtual void OnSetFocus(HWND hWnd) = 0;

		virtual const Ray3& GetWorldRay() = 0;

		virtual bool IsIn(const RECT& r) = 0;

		virtual void CursorToCenter() = 0;
		virtual void SetCursorPosition(const Vec2I& cursorPos) = 0;

		virtual void ClearRightDown() = 0;
	};
}

#endif //_fastbird_IMouse_header_included_