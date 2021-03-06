#pragma once
#include <Engine/RenderSteps.h>

namespace fastbird
{

class RenderPipeline : public ReferenceCounter
{
	bool mSteps[RenderSteps::Num];

public:
	RenderPipeline();
	RenderPipeline(const RenderPipeline& other);
	RenderPipeline(bool steps[]);
	~RenderPipeline();

protected:
	virtual void FinishSmartPtr();

public:

	RenderPipeline* Clone() const;
	virtual RenderPipeline& SetStep(RenderSteps::Enum step, bool enable);
	bool GetStep(RenderSteps::Enum step) const;

	virtual RenderPipeline& SetMaximum();
	virtual RenderPipeline& SetMinimum();

};

}