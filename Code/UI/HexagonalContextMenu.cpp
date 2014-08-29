#include <UI/StdAfx.h>
#include <UI/HexagonalContextMenu.h>
#include <UI/StaticText.h>
#include <UI/ImageBox.h>

namespace fastbird
{
	HexagonalContextMenu::HexagonalContextMenu()
		: mUpdateMaterialParams(true)
		, mMouseInHexaIdx(-1)
	{
		mUIObject = IUIObject::CreateUIObject(false);
		mUIObject->SetMaterial("es/Materials/UIHexagonal.material");
		mUIObject->mOwnerUI = this;
		mUIObject->mTypeString = ComponentType::ConvertToString(GetType());

		RegisterEventFunc(IEventHandler::EVENT_MOUSE_HOVER,
			std::bind(&HexagonalContextMenu::OnMouseHover, this, std::placeholders::_1));
		RegisterEventFunc(IEventHandler::EVENT_MOUSE_OUT,
			std::bind(&HexagonalContextMenu::OnMouseOut, this, std::placeholders::_1));

		for (int i = 0; i < 6; i++)
		{
			mHexaEnabled[i] = false;
			mHexaImages[i] = 0;
			mHexaStaticTexts[i] = 0;
		}

		mHexaOrigins[0] = Vec2(0.0f, 0.61f);
		mHexaOrigins[1] = Vec2(0.52f, 0.3f);
		mHexaOrigins[2] = Vec2(0.52f, -0.3f);
		mHexaOrigins[3] = Vec2(0.0f, -0.61f);
		mHexaOrigins[4] = Vec2(-0.52f, -0.3f);
		mHexaOrigins[5] = Vec2(-0.52f, 0.3f);
		
		

		Vec2 texcoords[4] = {
			Vec2(0.f, 1.f),
			Vec2(0.f, 0.f),
			Vec2(1.f, 1.f),
			Vec2(1.f, 0.f)
		};
		mUIObject->SetTexCoord(texcoords, 4);
	}

	HexagonalContextMenu::~HexagonalContextMenu()
	{

	}

	void HexagonalContextMenu::GatherVisit(std::vector<IUIObject*>& v)
	{
		if (!mVisible)
			return;
		v.push_back(mUIObject);
		__super::GatherVisit(v);
	}

	void HexagonalContextMenu::SetNPos(const fastbird::Vec2& pos) // normalized pos (0.0~1.0)
	{
		__super::SetNPos(pos);
		mUpdateMaterialParams = true;
	}

	void HexagonalContextMenu::SetHexaEnabled(unsigned index, unsigned cmdID /*= -1*/)
	{
		assert(index < 6);
		mHexaEnabled[index] = true;
		mUpdateMaterialParams = true;

		mCmdID[index] = cmdID;
	}

	void HexagonalContextMenu::DisableHexa(unsigned index)
	{
		assert(index < 6);
		mHexaEnabled[index] = false;
		mUpdateMaterialParams = true;

	}

	void HexagonalContextMenu::SetHexaText(unsigned index, const wchar_t* text)
	{
		assert(index < 6);
		if (!mHexaEnabled[index])
			return;
		if (!mHexaStaticTexts[index])
			mHexaStaticTexts[index] = (StaticText*)AddChild(
			mHexaOrigins[index].x*.5f + .5f, mHexaOrigins[index].y*-.5f + .5f, 0.16f, 0.16f, ComponentType::StaticText);

		mHexaStaticTexts[index]->SetVisible(true);
		mHexaStaticTexts[index]->SetAlign(ALIGNH::CENTER, ALIGNV::MIDDLE);
		mHexaStaticTexts[index]->SetProperty(UIProperty::TEXT_ALIGN, "center");
		mHexaStaticTexts[index]->SetProperty(UIProperty::TEXT_COLOR, "0.86667, 1.0, 0.1843, 1");
		mHexaStaticTexts[index]->SetText(text);
		mHexaStaticTexts[index]->DisableEvent(IEventHandler::EVENT_MOUSE_CLICK);
	}

	void HexagonalContextMenu::SetHexaImageIcon(unsigned index, const char* atlas, const char* region)
	{
		mHexaImages[index] = (ImageBox*)AddChild(mHexaOrigins[index].x*.5f + .5f, mHexaOrigins[index].y*-.5f + .5f, 0.2f, 0.2f, ComponentType::ImageBox);
	}

	void HexagonalContextMenu::ClearHexa()
	{
		for (int i = 0; i < 6; i++)
		{
			mHexaEnabled[i] = false;
			if (mHexaStaticTexts[i])
			{
				mHexaStaticTexts[i]->SetVisible(false);
			}
			if (mHexaImages[i])
			{
				mHexaImages[i]->SetVisible(false);
			}
			
			mCmdID[i] = -1;		
		}
		mUpdateMaterialParams = true;
	}

	void HexagonalContextMenu::OnMouseHover(void* arg)
	{
		SetCursor(WinBase::mCursorOver);
	}
	void HexagonalContextMenu::OnMouseOut(void* arg)
	{

	}

	void HexagonalContextMenu::UpdateMaterialParameters()
	{
		if (mUpdateMaterialParams)
		{
			mUpdateMaterialParams = false;
			IMaterial* mat = mUIObject->GetMaterial();
			assert(mat);
			Vec4 param[3];
			param[0] = Vec4(mHexaEnabled[0] ? 1.0f : 0.0f, mHexaEnabled[1] ? 1.0f : 0.0f,
				mHexaEnabled[2] ? 1.0f : 0.0f, mHexaEnabled[3] ? 1.0f : 0.0f);
			param[1] = Vec4(mHexaEnabled[4] ? 1.0f : 0.0f, mHexaEnabled[5] ? 1.0f : 0.0f,
				mWNSize.x, mWNSize.y);
			param[2] = Vec4(mWNPos.x, mWNPos.y, 0.0f, 0.0f);
			for (int i = 0; i < 3; ++i)
				mat->SetMaterialParameters(i, param[i]);
		}
	}

	bool HexagonalContextMenu::IsIn(const Vec2& mouseNormpos)
	{
		Vec2 mouse = (mouseNormpos - mWNPos) / mWNSize;
		mouse = mouse*2.0f - 1.0f;
		mouse.y = -mouse.y;
		mMouseInHexaIdx = -1;
		for (int i = 0; i<6; ++i)
		{
			if (!mHexaEnabled[i])
				continue;
			// world means just entire Hexagonal Area.
			Vec2 worldMouse = mHexaOrigins[i] - mouse;
			worldMouse = Abs(worldMouse);
			float m = std::max(worldMouse.x + worldMouse.y*0.57735f, worldMouse.y*1.1547f) - 0.2f;
			
			if (m<=0.1)
			{
				mMouseInHexaIdx = i;
				break;
			}
		}
		return mMouseInHexaIdx != -1;
	}

	//-----------------------------------------------------------------------
	void HexagonalContextMenu::SetCmdID(unsigned idx, unsigned id)
	{
		if (idx < 6)
		{
			mCmdID[idx] = id;
		}
		else
		{
			assert(0);
		}
	}
	
	//-----------------------------------------------------------------------
	unsigned HexagonalContextMenu::GetCurCmdID()
	{
		if (mMouseInHexaIdx>= 0 && mMouseInHexaIdx < 6)
		{
			return mCmdID[mMouseInHexaIdx];
		}

		return -1;
	}
}