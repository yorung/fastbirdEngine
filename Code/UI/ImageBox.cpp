#include <UI/StdAfx.h>
#include <UI/ImageBox.h>

namespace fastbird
{

ImageBox::ImageBox()
	: mTextureAtlas(0)
	, mAtlasRegion(0)
	, mUseHighlight(false)
	, mKeepImageRatio(true)
	, mFrameImage(0)
{
	mUIObject = IUIObject::CreateUIObject(false);
	mUIObject->SetMaterial("es/Materials/UIImageBox.material");
	mUIObject->mOwnerUI = this;
	mUIObject->mTypeString = ComponentType::ConvertToString(GetType());

	RegisterEventFunc(IEventHandler::EVENT_MOUSE_HOVER, 
		std::bind(&ImageBox::OnMouseHover, this, std::placeholders::_1));
	RegisterEventFunc(IEventHandler::EVENT_MOUSE_OUT, 
		std::bind(&ImageBox::OnMouseOut, this, std::placeholders::_1));
}

ImageBox::~ImageBox()
{
}

void ImageBox::CalcUV(const Vec2I& textureSize)
{
	float width = (float)textureSize.x;
	float height = (float)textureSize.y;
	float imgRatio = width / height;
	const RECT& uiRect = mUIObject->GetRegion();
	float uiRatio = (uiRect.right - uiRect.left) /
		(float)(uiRect.bottom - uiRect.top);
	if (uiRatio == imgRatio || !mKeepImageRatio)
	{
		Vec2 texcoords[4] = {
			Vec2(0.f, 1.f),
			Vec2(0.f, 0.f),
			Vec2(1.f, 1.f),
			Vec2(1.f, 0.f)
		};
		mUIObject->SetTexCoord(texcoords, 4);
	}
	else
	{
		float halfu = (uiRatio / imgRatio) * .5f;
		Vec2 texcoords[4] = {
			Vec2(0.5f - halfu, 1.f),
			Vec2(0.5f - halfu, 0.f),
			Vec2(0.5f + halfu, 1.f),
			Vec2(0.5f + halfu, 0.f)
		};
		mUIObject->SetTexCoord(texcoords, 4);
	}
}

void ImageBox::SetTexture(const char* file)
{
	if (!file || strlen(file) == 0)
		return;
	mImageFile = file;
	ITexture* pTexture = gEnv->pRenderer->CreateTexture(file);
	SetTexture(pTexture);
}

void ImageBox::SetTexture(ITexture* pTexture)
{
	if (!pTexture)
		return;
	SAMPLER_DESC sd;
	sd.AddressU = TEXTURE_ADDRESS_BORDER;
	sd.AddressV = TEXTURE_ADDRESS_BORDER;
	sd.AddressW = TEXTURE_ADDRESS_BORDER;
	mUIObject->GetMaterial()->SetTexture(pTexture, BINDING_SHADER_PS, 0, sd);
	CalcUV(pTexture->GetSize());
}

void ImageBox::SetTextureAtlasRegion(const char* atlas, const char* region)
{
	mTextureAtlas = gEnv->pRenderer->GetTextureAtlas(atlas);
	if (mTextureAtlas)
	{
		mTexture = mTextureAtlas->mTexture->Clone();
		mAtlasRegion = mTextureAtlas->GetRegion(region);
		SAMPLER_DESC sdesc;
		sdesc.Filter = TEXTURE_FILTER_MIN_MAG_MIP_POINT;
		mUIObject->GetMaterial()->SetTexture(mTexture, 
			BINDING_SHADER_PS, 0, sdesc);
		if (mAtlasRegion)
		{
			Vec2 texcoords[4];
			mAtlasRegion->GetQuadUV(texcoords);
			mUIObject->SetTexCoord(texcoords, 4);
			DWORD colors[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
			mUIObject->SetColors(colors, 4);
		}
	}
}

void ImageBox::ChangeRegion(TextureAtlasRegion* region)
{
	mAtlasRegion = region;
	if (mAtlasRegion)
	{
		Vec2 texcoords[4];
		mAtlasRegion->GetQuadUV(texcoords);
		mUIObject->SetTexCoord(texcoords, 4);
	}	
}

void ImageBox::ChangeRegion(const char* region)
{
	assert(mTextureAtlas);
	if (mTextureAtlas)
	{
		mAtlasRegion = mTextureAtlas->GetRegion(region);
		if (mAtlasRegion)
		{
			Vec2 texcoords[4];
			mAtlasRegion->GetQuadUV(texcoords);
			mUIObject->SetTexCoord(texcoords, 4);
		}
	}

}

void ImageBox::GatherVisit(std::vector<IUIObject*>& v)
{
	if (!mVisible)
		return;
	v.push_back(mUIObject);
	__super::GatherVisit(v);
}

void ImageBox::OnSizeChanged()
{
	__super::OnSizeChanged();
	if (!mAtlasRegion)
	{
		auto texture = mUIObject->GetMaterial()->GetTexture(BINDING_SHADER_PS, 0);
		if (texture)
			CalcUV(texture->GetSize());
	}
}

void ImageBox::Highlight(bool enable)
{
	if (!mUseHighlight)
		return;

	if (enable)
	{
		mUIObject->GetMaterial()->SetEmissiveColor(0.15f, 0.95f, 0.9f, 1);
	}
	else
	{
		mUIObject->GetMaterial()->SetEmissiveColor(0, 0, 0, 0);
	}
}

void ImageBox::OnMouseHover(void* arg)
{
}

void ImageBox::OnMouseOut(void* arg)
{
}

bool ImageBox::SetProperty(UIProperty::Enum prop, const char* val)
{
	switch (prop)
	{
	case UIProperty::TEXTUREATLAS:
	{
									 mTextureAtlasFile = val;
									 return true;
	}
		break;

	case UIProperty::REGION:
	{
							   if (!mTextureAtlasFile.empty())
							   {
								   SetTextureAtlasRegion(mTextureAtlasFile.c_str(), val);
							   }
							   return true;
	}
		break;

	case UIProperty::TEXTURE_FILE:
	{
									 SetTexture(val);
									 return true;
	}

	case UIProperty::KEEP_IMAGE_RATIO:
	{
										 SetKeepImageRatio(StringConverter::parseBool(val, true));
										 return true;
	}
	case UIProperty::FRAME_IMAGE:
	{
									if (!mFrameImage)
									{
										mFrameImage = CreateImageBox();
									}
									if_assert_pass(!mTextureAtlasFile.empty())
									{
										mFrameImage->SetTextureAtlasRegion(mTextureAtlasFile.c_str(), val);
										if (strlen(val) == 0)
										{
											mFrameImage->SetVisible(false);
										}
										else
										{
											mFrameImage->SetVisible(true);
										}
									}
									return true;
	}
	}

	return __super::SetProperty(prop, val);
}

void ImageBox::SetKeepImageRatio(bool keep)
{
	mKeepImageRatio = keep;
	if (!mAtlasRegion)
	{
		auto texture = mUIObject->GetMaterial()->GetTexture(BINDING_SHADER_PS, 0);
		if (texture)
			CalcUV(texture->GetSize());
	}
}

ImageBox* ImageBox::CreateImageBox()
{
	auto image = (ImageBox*)AddChild(0, 0, 1.0f, 1.0f, ComponentType::ImageBox);
	return image;
}


}
