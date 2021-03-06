#include <Engine/StdAfx.h>
#include <Engine/MeshGroup.h>
#include <Engine/MeshObject.h>
#include <Engine/Animation.h>
#include <Engine/FBCollisionShape.h>

namespace fastbird
{
MeshGroup::MeshGroup()
	: mAuxCloned(0)
	, mLastUpdateFrame(0)
	, mRootAnimated(false)
	, mCollisionsCloned(0)
	, mRootAuxCloned(0)
{
}

MeshGroup::~MeshGroup()
{
	for (auto& it : mCollisions)
	{
		FB_DELETE(it);
	}
}
		
void MeshGroupPreRender()
{
}

IMaterial* MeshGroup::GetMaterial(int pass /*= RENDER_PASS::PASS_NORMAL*/) const
{
	if (!mMeshObjects.empty())
	{
		return mMeshObjects[0].first->GetMaterial();
	}
	return 0;
}

// transform : parent space
size_t MeshGroup::AddMesh(IMeshObject* mesh, const Transformation& transform, size_t parent)
{
	mMeshObjects.push_back(MESH_OBJECTS::value_type(mesh, transform));
	mLocalTransforms.push_back(Transformation::IDENTITY);
	mChanges.push_back(true);
	
	size_t idx = mMeshObjects.size() -1;
	if (idx==0)
	{
		*mBoundingVolume = *(mesh->GetBoundingVolume());
	}
	else
	{
		mBoundingVolume->Merge(mesh->GetBoundingVolume());
	}
	mHierarchyMap[idx].mParentIndex = parent;
	mHierarchyMap[idx].mMyIndex = idx;
	if (parent !=-1)
	{
		mHierarchyMap[parent].mChildren.push_back(idx);
	}

	*mBoundingVolumeWorld = *mBoundingVolume;
	return idx;
}

const char* MeshGroup::GetNameOfMesh(size_t idx)
{
	assert(idx < mMeshObjects.size());
	return mMeshObjects[idx].first->GetName();
}

size_t MeshGroup::GetNumMeshes() const
{
	return mMeshObjects.size();
}

void MeshGroup::AddMeshRotation(size_t idx, const Quat& rot)
{
	assert(idx < mLocalTransforms.size());
	mLocalTransforms[idx].AddRotation(rot);
	mChanges[idx] = true;
	mTransformChanged = true;
	if (idx == 0)
		mRootAnimated = true;
}

const Quat& MeshGroup::GetMeshRotation(size_t idx) const
{
	assert(idx < mLocalTransforms.size());
	return mLocalTransforms[idx].GetRotation();
}

void MeshGroup::SetMeshRotation(size_t idx, const Quat& rot)
{
	assert(idx < mLocalTransforms.size());
	mLocalTransforms[idx].SetRotation(rot);
	mChanges[idx] = true;
	mTransformChanged = true;
	if (idx == 0)
		mRootAnimated = true;
}

const Vec3& MeshGroup::GetMeshOffset(size_t idx) const
{
	assert(idx < mMeshObjects.size());
	// this would be wrong in hierarcy which has more than two depths.
	return mMeshObjects[idx].second.GetTranslation();
}

IObject* MeshGroup::Clone() const
{
	MeshGroup* cloned = FB_NEW(MeshGroup);
	SpatialObject::Clone(cloned);

	for (auto it : mMeshObjects)
	{
		cloned->mMeshObjects.push_back(MESH_OBJECTS::value_type((IMeshObject*)it.first->Clone(), it.second));
		cloned->mLocalTransforms.push_back(Transformation::IDENTITY);
		cloned->mChanges.push_back(true);
		cloned->mHierarchyMap = mHierarchyMap;
	}
	cloned->mRootAuxCloned = mRootAuxCloned ? mRootAuxCloned : &mRootAuxil;
	cloned->mAuxCloned = mAuxCloned ? mAuxCloned : (AUXIL_MAP*)&mAuxil;
	cloned->mCollisionsCloned = mCollisionsCloned ? mCollisionsCloned : (COLLISION_SHAPES*)&mCollisions;
	return cloned;
}

void MeshGroup::Delete()
{
	FB_DELETE(this);
}

void MeshGroup::PreRender()
{
	if (mObjFlag & IObject::OF_HIDE)
		return;

	if (mLastPreRendered == gFBEnv->mFrameCounter)
		return;
	mLastPreRendered = gFBEnv->mFrameCounter;

	UpdateTransform(); // and prerender children.
}

void MeshGroup::Render()
{
	if (mObjFlag & IObject::OF_HIDE)
		return;

	FB_FOREACH(it, mMeshObjects)
	{
		it->first->Render();
	}
}

void MeshGroup::PostRender()
{
	if (mObjFlag & IObject::OF_HIDE)
		return;

	FB_FOREACH(it, mMeshObjects)
	{
		it->first->PostRender();
	}
}

const AUXILIARIES* MeshGroup::GetAuxiliaries(size_t idx) const
{ 
	if (idx == -1)
	{
		if (mRootAuxCloned)
		{
			return mRootAuxCloned;
		}
		else
		{
			return &mRootAuxil;
		}
	}
	else
	{
		if (mAuxCloned)
		{
			return (*mAuxCloned).Find(idx) == (*mAuxCloned).end() ? 0 : &(*mAuxCloned)[idx];
		}
		else
		{
			return mAuxil.Find(idx) == mAuxil.end() ? 0 : &mAuxil[idx];
		}
	}
}
void MeshGroup::SetAuxiliaries(size_t idx, const AUXILIARIES& aux)
{ 
	assert(!mAuxCloned);
	if (idx == -1)
	{
		mRootAuxil = aux;
	}
	else
	{
		mAuxil[idx] = aux;
	}
}

void MeshGroup::SetCollisionShapes(COLLISION_INFOS& colInfos)
{
	DeleteCollisionShapes();
	for (const auto& var : colInfos)
	{
		mCollisions.push_back(FB_NEW(FBCollisionShape(var.mColShapeType, var.mTransform, var.mCollisionMesh)));
	}
}

void MeshGroup::DeleteCollisionShapes()
{
	for (auto cs : mCollisions)
	{
		FB_DELETE(cs);
	}
	mCollisions.clear();
}

void MeshGroup::AddAuxiliary(size_t idx, const AUXILIARIES::value_type& v)
{
	assert(!mAuxCloned);
	mAuxil[idx].push_back(v);
}

void MeshGroup::AddCollisionShape(size_t idx, std::pair<FBColShape::Enum, Transformation>& data)
{
	mMeshObjects[idx].first->AddCollisionShape(data);
}

void MeshGroup::SetCollisionMesh(size_t idx, IMeshObject* colMesh)
{
	mMeshObjects[idx].first->SetCollisionMesh(colMesh);
}

void MeshGroup::UpdateTransform(bool force)
{
	Timer::FRAME_PRECISION f = gFBEnv->pTimer->GetFrame();
	if (force || mLastUpdateFrame < f)
	{
		mLastUpdateFrame = f;

		size_t num = mChanges.size();
		for (size_t i = 0; i < num; ++i)
		{
			auto anim = mMeshObjects[i].first->GetAnimation();
			bool animated = anim && anim->Changed();
			if (mChanges[i] || mTransformChanged || animated)
			{				
				for (unsigned c = i; c < num; ++c){
					const Hierarchy& h = mHierarchyMap[c];
					if (h.mParentIndex == i)
						mChanges[c] = true;
				}
				// calc transform
				const Hierarchy& h = mHierarchyMap[i];
				Transformation transform;
				if (h.mParentIndex != -1)
				{
					transform = mMeshObjects[h.mParentIndex].first->GetAnimatedTransform();
					transform = transform * mMeshObjects[i].second * mLocalTransforms[i];
				}
				else
				{
					if (mRootAnimated)
						transform = mTransformation * mMeshObjects[i].second * mLocalTransforms[i];
					else
						// for parents mesh, don't need to  multiply mLocalTransforms[i];
						transform = mTransformation * mMeshObjects[i].second;
				}

				mMeshObjects[i].first->SetTransform(transform);
				mChanges[i] = false;
			}
			mMeshObjects[i].first->PreRender();
		}
		mTransformChanged = false;
	}
}

Transformation MeshGroup::GetToLocalTransform(unsigned meshIdx)
{
	Transformation transform = mMeshObjects[meshIdx].second;
	const Hierarchy& h = mHierarchyMap[meshIdx];
	auto parentIdx = h.mParentIndex;
	while (parentIdx != -1)
	{
		transform = mMeshObjects[parentIdx].second * transform;
		parentIdx = mHierarchyMap[parentIdx].mParentIndex;
	}
	Transformation invTransform;
	transform.Inverse(invTransform);
	return invTransform;
}

Transformation MeshGroup::GetToLocalTransform(const char* meshName)
{
	unsigned i = 0;
	for (auto& it : mMeshObjects)
	{
		auto meshObj = it.first;
		if (strcmp(meshObj->GetName(), meshName) == 0)
		{
			return GetToLocalTransform(i);
		}
		++i;
	}
	return Transformation();
}

Transformation MeshGroup::GetLocalMeshTransform(unsigned meshIdx)
{
	const Hierarchy& h = mHierarchyMap[meshIdx];
	auto parentIdx = h.mParentIndex;
	if (parentIdx != -1){
		Transformation transform = mMeshObjects[parentIdx].first->GetTransform() * mMeshObjects[meshIdx].second;
		return transform;
	}
	else{
		return mMeshObjects[meshIdx].second;
	}
}

//---------------------------------------------------------------------------
void MeshGroup::SetEnableHighlight(bool enable)
{
	FB_FOREACH(it, mMeshObjects)
	{
		it->first->SetEnableHighlight(enable);
	}
}

//---------------------------------------------------------------------------
void MeshGroup::SetAnimationData(const char* meshName, const AnimationData& anim, const char* actionFile)
{
	unsigned i = 0;
	for (auto& it : mMeshObjects)
	{
		auto meshObj = it.first;
		if (strcmp(meshObj->GetName(), meshName) == 0)
		{
			meshObj->SetAnimationData(anim, actionFile);
			return;
		}
		++i;
	}
	assert(0);
	Error("Mesh group doesn't have a name %s", meshName);
}

void MeshGroup::PlayAction(const std::string& name, bool immediate, bool reverse)
{
	for (auto& it : mMeshObjects)
	{
		auto meshObj = it.first;
		auto anim = meshObj->GetAnimation();
		if (anim)
		{
			anim->PlayAction(name, immediate, reverse);
		}
	}
}

bool MeshGroup::IsActionDone(const char* action) const
{
	for (auto& it : mMeshObjects)
	{
		auto meshObj = it.first;
		auto anim = meshObj->GetAnimation();
		if (anim)
		{
			if (anim->IsActionDone(action))
				return true;
		}
	}
	return false;
}

void MeshGroup::SetAlpha(float alpha)
{
	for (auto& it : mMeshObjects)
	{
		it.first->SetAlpha(alpha);		
	}
}

void MeshGroup::SetForceAlphaBlending(bool enable, float alpha, float forceGlow, bool disableDepth)
{
	for (auto& it : mMeshObjects)
	{
		it.first->SetForceAlphaBlending(enable, alpha, forceGlow, disableDepth);
	}
}

void MeshGroup::SetAmbientColor(const Color& color)
{
	for (auto& it : mMeshObjects)
	{
		it.first->SetAmbientColor(color);
	}
}

IMeshObject* MeshGroup::GetMeshObject(unsigned idx)
{
	if_assert_pass(idx < mMeshObjects.size())
	{
		return mMeshObjects[idx].first;
	}
	return 0;
}


unsigned MeshGroup::GetNumCollisionShapes() const
{
	if (mCollisionsCloned)
	{
		return mCollisionsCloned->size();
	}

	return mCollisions.size();
}

bool MeshGroup::IsPlayingAction() const{
	auto numMeshes = GetNumMeshes();
	for (unsigned i = 0; i < numMeshes; ++i){
		if (mMeshObjects[i].first->IsPlayingAction())
			return true;
	}

	return false;
}

unsigned MeshGroup::GetNumCollisionShapes(unsigned idx) const
{
	if_assert_pass(idx < mMeshObjects.size())
	{
		return mMeshObjects[idx].first->GetNumCollisionShapes();
	}
	return 0;
}

const FBCollisionShape* MeshGroup::GetCollisionShape(unsigned idx) const
{
	if (mCollisionsCloned)
	{
		assert(idx < mCollisionsCloned->size());
		return (*mCollisionsCloned)[idx];
	}
	else
	{
		assert(idx < mCollisions.size());
		return mCollisions[idx];
	}
}

}