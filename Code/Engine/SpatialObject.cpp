#include <Engine/StdAfx.h>
#include <Engine/SpatialObject.h>
#include <Engine/IScene.h>
#include <Engine/ICamera.h>
#include <Engine/Animation.h>
using namespace fastbird;

//----------------------------------------------------------------------------
SpatialObject::SpatialObject()
	:mDistToCam(-1.f)
	, mPrevPos(0, 0, 0)
	, mAnimData(0)
	, mAnimOwner(false)
	, mTransformChanged(true)
	, mAnim(0)
{
}

//----------------------------------------------------------------------------
SpatialObject::~SpatialObject()
{
	while (!mCameraTargetingMe.empty())
	{
		auto it = mCameraTargetingMe.end() - 1;
		(*it)->SetTarget(0);
	}

	DetachFromScene(true);

	if (mAnimData && mAnimOwner)
		FB_DELETE(mAnimData);

	if (mAnim)
		FB_DELETE(mAnim);
}

//----------------------------------------------------------------------------
void SpatialObject::Clone(IObject* cloned) const
{
	__super::Clone(cloned);
	SpatialObject* object = (SpatialObject*)cloned;
	object->mTransformation = mTransformation;
	object->mTransformation.SetTranslation(mTransformation.GetTranslation()+Vec3(1.f, 0, 0));
	object->mDistToCam = mDistToCam;
	object->mAnimData = mAnimData;
	if (mAnimData)
	{
		object->mAnim = FB_NEW(Animation);
		object->mAnim->SetAnimationData(mAnimData);
	}
}

//----------------------------------------------------------------------------
void SpatialObject::SetPos(const Vec3& pos)
{
	if (mBoundingVolume && mBoundingVolumeWorld)
		mBoundingVolumeWorld->SetCenter(mBoundingVolume->GetCenter() + pos);
	mPrevPos = mTransformation.GetTranslation();
	mTransformation.SetTranslation(pos);
	mTransformChanged = true;
}

//----------------------------------------------------------------------------
void SpatialObject::SetRot(const Quat& rot)
{
	mTransformation.SetRotation(rot);
	mTransformChanged = true;
}

void SpatialObject::SetScale(const Vec3& scale)
{
	// for fix precision problem.
	if (mBoundingVolume && mBoundingVolumeWorld)
		mBoundingVolumeWorld->SetRadius(mBoundingVolume->GetRadius() * std::max(scale.x, std::max(scale.y, scale.z)));
	mTransformation.SetScale(scale);
	mTransformChanged = true;
}

//----------------------------------------------------------------------------
// dir should be already normalized.
void SpatialObject::SetDir(const Vec3& dir)
{
	mTransformation.SetDir(dir);
	mTransformChanged = true;
}

void SpatialObject::SetDirAndRight(const Vec3& dir, const Vec3& right)
{
	mTransformation.SetDirAndRight(dir, right);
	mTransformChanged = true;
}

//----------------------------------------------------------------------------
const Vec3& SpatialObject::GetPos() const
{
	return mTransformation.GetTranslation();
}

//----------------------------------------------------------------------------
const Quat& SpatialObject::GetRot() const
{
	return mTransformation.GetRotation();
}

//----------------------------------------------------------------------------
void SpatialObject::SetTransform(const Transformation& t)
{
	if (mBoundingVolume && mBoundingVolumeWorld)
	{
		auto& scalev = mTransformation.GetScale();
		float scale = std::max(scalev.x, std::max(scalev.y, scalev.z));
		mBoundingVolumeWorld->SetCenter(mBoundingVolume->GetCenter() * scale + t.GetTranslation());
	}
		
	mPrevPos = mTransformation.GetTranslation();
	mTransformation = t;
	mTransformChanged = true;
}

//----------------------------------------------------------------------------
void SpatialObject::SetDistToCam(float dist)
{
	mDistToCam = dist;
}

//----------------------------------------------------------------------------
float SpatialObject::GetDistToCam() const
{
	return mDistToCam;
}

void SpatialObject::AttachToScene()
{
	auto scene = gFBEnv->pRenderer->GetMainScene();
	if (scene)
	{
		scene->AttachObject(this);
	}
	else
	{
		Error(FB_DEFAULT_DEBUG_ARG, "No main scene found!");
	}
}

void SpatialObject::DetachFromScene(bool includingRtt)
{
	unsigned limit = mScenes.size();
	unsigned count = 0;
	for (unsigned i = 0; i < mScenes.size() && count < limit;)
	{
		auto scene = mScenes[i];
		if (includingRtt)
		{
			scene->DetachObject(this);
		}
		else
		{
			if (!scene->IsRttScene())
			{
				scene->DetachObject(this);
			}
			else
			{
				++i;
			}
		}
		++count;
	}
}

void SpatialObject::AddCameraTargetingMe(ICamera* pCam)
{
	auto it = std::find(mCameraTargetingMe.begin(), mCameraTargetingMe.end(), pCam);
	if (it != mCameraTargetingMe.end())
		return;
	mCameraTargetingMe.push_back(pCam);
}

void SpatialObject::RemoveCameraTargetingMe(ICamera* pCam)
{
	auto it = std::find(mCameraTargetingMe.begin(), mCameraTargetingMe.end(), pCam);
	if (it != mCameraTargetingMe.end())
		mCameraTargetingMe.erase(it);
}

void SpatialObject::SetAnimationData(const AnimationData& animData, const char* actionFile)
{
	if (mAnimData && mAnimOwner)
		FB_DELETE(mAnimData);
	mAnimData = FB_NEW(AnimationData);
	*mAnimData = animData;
	mAnimOwner = true;

	mAnimData->ParseAction(actionFile);
}

void SpatialObject::PreRender()
{
	if (mAnim)
		mAnim->Update(gpTimer->GetDeltaTime());
}