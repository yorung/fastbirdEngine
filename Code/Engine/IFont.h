#pragma once
#include <Engine/Object.h>
#include <CommonLib/SmartPtr.h>
namespace fastbird
{
	enum FONT_ALIGN
	{
		FONT_ALIGN_LEFT = 0,
		FONT_ALIGN_CENTER,
		FONT_ALIGN_RIGHT,
		FONT_ALIGN_JUSTIFY
	};

	//------------------------------------------------------------------------
	class IFont : public Object
	{
	public:
		virtual ~IFont(){}

		enum EFontTextEncoding
		{
			NONE,
			UTF8,
			UTF16
		};

		virtual int Init(const char *fontFile) = 0;
		virtual void SetTextEncoding(EFontTextEncoding encoding) = 0;
		virtual void Write(float x, float y, float z, unsigned int color, 
			const char *text, int count, FONT_ALIGN mode) = 0;

		virtual void SetHeight(float h) = 0;
		virtual void SetBackToOrigHeight() = 0;
		virtual float GetHeight() const = 0;
		virtual float GetBaseHeight() const = 0;
		virtual float GetTextWidth(const char *text, int count = -1, float *minY = 0, float *maxY = 0) = 0;
		virtual std::wstring InsertLineFeed(const char *text, int count, unsigned wrapAt, float* outWidth, unsigned* outLines) = 0;
		virtual void PrepareRenderResources() = 0;
		virtual void SetRenderStates(bool depthEnable = false, bool scissorEnable = false) = 0;
		virtual void SetRenderTargetSize(const Vec2I& rtSize) = 0;
		virtual void RestoreRenderTargetSize() = 0;
		virtual std::wstring StripTags(const wchar_t* text) = 0;

	};
}