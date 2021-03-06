#pragma once

#include <UI/Container.h>

namespace fastbird
{
	class IRenderTarget;
class ImageBox : public Container
{
public:
	ImageBox();
	~ImageBox();

	virtual void OnCreated();
	// IWinBase
	virtual ComponentType::Enum GetType() const { return ComponentType::ImageBox; }
	virtual void GatherVisit(std::vector<IUIObject*>& v);
	virtual void OnSizeChanged();
	virtual void OnParentSizeChanged();
	void OnAnySizeChanged();
	virtual void OnStartUpdate(float elapsedTime);
	virtual bool OnInputFromHandler(IMouse* mouse, IKeyboard* keyboard);

	// ImageBox;
	virtual void SetTexture(const char* file);
	virtual void SetTexture(ITexture* pTexture);
	// image box will own the rt.
	virtual void SetRenderTargetTexture(IRenderTarget* rt);
	virtual IRenderTarget* GetRenderTargetTexture() const { return mRenderTarget; }
	virtual void SetUseHighlight(bool use) { mUseHighlight = use; }
	// or
	virtual const Vec2I& SetTextureAtlasRegion(const char* atlas, const char* region);
	virtual void SetTextureAtlasRegions(const char* atlas, const std::vector<std::string>& data);
	virtual void ChangeRegion(TextureAtlasRegion* region);
	virtual void ChangeRegion(const char* region);
	virtual TextureAtlasRegion* GetTextureAtlasRegion() const { return mAtlasRegion; }
	virtual bool IsAnimated() const;
	virtual void Highlight(bool enable);
	virtual bool SetProperty(UIProperty::Enum prop, const char* val);
	virtual bool GetProperty(UIProperty::Enum prop, char val[], unsigned bufsize, bool notDefaultOnly);
	virtual void SetVisibleInternal(bool visible);

	void SetKeepImageRatio(bool keep);
	void SetUVRot(bool set);
	void SetCenterUVMatParam();
	void DrawAsFixedSizeCenteredAt(const Vec2I& wpos);
	void DrawAsFixedSizeAtCenter();
	void DrawAsFixedSize();
	void SetDesaturate(bool desat);
	void SetAmbientColor(const Vec4& color);
	void SetSpecularColor(const Vec4& color);
	void SetDefaultImageAtlasPathIfNotSet();
	void SetBorderAlpha(bool use);	

private:

	ImageBox* ImageBox::CreateImageBox();
	void CalcUV(const Vec2I& textureSize);

	void SetAlphaTextureAutoGenerated(bool set);


private:
	std::string mTextureAtlasFile;
	std::string mStrRegion;
	std::string mStrRegions;
	std::string mStrFrameImage;
	std::string mImageFile; 
	TextureAtlas* mTextureAtlas;
	TextureAtlasRegion* mAtlasRegion;
	std::vector<TextureAtlasRegion*> mAtlasRegions;
	IRenderTarget* mRenderTarget;
	// should not be smart pointer
	// material will hold a reference of this image.
	ITexture* mTexture;
	bool mUseHighlight;
	bool mKeepImageRatio;
	ImageBox* mFrameImage;	
	float mSecPerFrame;
	float mPlayingTime;
	unsigned mCurFrame;
	bool mAnimation;
	bool mImageFixedSize;	
	bool mColorOveraySet;
	bool mImageRot;
};
}