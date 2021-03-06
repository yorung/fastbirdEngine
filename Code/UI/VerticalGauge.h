#pragma once
#include <UI/WinBase.h>

namespace fastbird
{
class IUIObject;

class VerticalGauge : public WinBase
{
public:
	VerticalGauge();
	virtual ~VerticalGauge();

	// IWinBase
	virtual ComponentType::Enum GetType() const { return ComponentType::VerticalGauge; }
	virtual void GatherVisit(std::vector<IUIObject*>& v);
	virtual void OnStartUpdate(float elapsedTime);

	virtual void SetPercentage(float p);
	virtual void SetMaximum(float m);
	virtual void Blink(bool blink);
	virtual void SetGaugeColor(const Color& color);
	virtual void SetBlinkColor(const Color& color);

	void SetTextureAtlasRegion(UIProperty::Enum, const char* region);

	virtual bool SetProperty(UIProperty::Enum prop, const char* val);
	virtual bool GetProperty(UIProperty::Enum prop, char val[], unsigned bufsize, bool notDefaultOnly);

private:
	float mPercentage;
	float mMaximum;
	Color mGaugeColor;
	Color mBlinkColor;
	bool mBlink;
	float mBlinkSpeed;

	std::string mTextureAtlasFile;
	TextureAtlas* mTextureAtlas;
	TextureAtlasRegion* mAtlasRegions[2];
	ITexture* mTextures[2];

	bool mMaterialUsingImage;
	bool mHorizontalFlip;

	std::string mRegionFilled;
	std::string mRegionNotFilled;
	std::string mStrGaugeBorderColor;

};
}
