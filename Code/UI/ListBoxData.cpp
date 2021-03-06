#include <UI/StdAfx.h>
#include <UI/ListBoxData.h>

namespace fastbird{
	ListBoxData::ListBoxData(ListItemDataType::Enum type, const wchar_t* text, bool checked)
		: mDataType(type)
		, mChecked(checked)
		, mKey(-1)
	{
		mNumber.i = 0;
		if (text)
			mText = text;
	}

	ListBoxData::ListBoxData()
		: mDataType(ListItemDataType::Unknown)
		, mChecked(false)
		, mKey(-1)
	{

	}

	ListBoxData::~ListBoxData()
	{

	}

	bool ListBoxData::IsTextData() const
	{
		return mDataType == ListItemDataType::String ||
			mDataType == ListItemDataType::TextField ||
			mDataType == ListItemDataType::TexturePath ||
			mDataType == ListItemDataType::TextureRegion;
	}

	bool ListBoxData::CanHaveFocus() const{
		return mDataType == ListItemDataType::TextField || mDataType == ListItemDataType::CheckBox;
	}

	void ListBoxData::SetTexture(ITexture* texture)
	{
		mTexture = texture;
		mDataType = ListItemDataType::TexturePath;
	}

	void ListBoxData::SetInt(int number){
		mNumber.i = number;
		mDataType = ListItemDataType::NumericUpDown;
	}

	void ListBoxData::SetFloat(float f){
		mNumber.f = f;
		mDataType = ListItemDataType::HorizontalGauge;
	}
}