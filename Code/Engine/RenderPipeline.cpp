#include <Engine/StdAfx.h>
#include <Engine/RenderPipeline.h>

namespace fastbird
{

RenderPipeline::RenderPipeline()
{
	SetMinimum();
}

RenderPipeline::RenderPipeline(bool steps[])
{
	memcpy(mSteps, steps, sizeof(mSteps));
}

RenderPipeline::RenderPipeline(const RenderPipeline& other)
{
	memcpy(mSteps, other.mSteps, sizeof(mSteps));
}

RenderPipeline::~RenderPipeline()
{

}

void RenderPipeline::FinishSmartPtr(){
	FB_DELETE(this);
}

RenderPipeline* RenderPipeline::Clone() const
{
	auto newPipeline = FB_NEW(RenderPipeline)(*this);
	return newPipeline;
}

RenderPipeline& RenderPipeline::SetStep(RenderSteps::Enum step, bool enable)
{
	mSteps[step] = enable;
	return *this;
}

bool RenderPipeline::GetStep(RenderSteps::Enum step) const
{
	return mSteps[step];
}

RenderPipeline& RenderPipeline::SetMaximum()
{
	for (unsigned i = 0; i < RenderSteps::Num; ++i)
	{
		mSteps[i] = true;
	}
	return *this;
}

RenderPipeline& RenderPipeline::SetMinimum()
{
	memset(mSteps, 0, sizeof(mSteps));
	mSteps[RenderSteps::ShadowMap] = true;
	mSteps[RenderSteps::Depth] = true;
	return *this;
}

}