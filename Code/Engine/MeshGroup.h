#pragma once
#include <Engine/IMeshGroup.h>
#include <CommonLib/FBColShape.h>

namespace fastbird
{
	class MeshGroup : public IMeshGroup
	{
	public:
		MeshGroup();
		virtual ~MeshGroup();
		
		virtual IObject* Clone() const;
		virtual void PreRender();
		virtual void Render();
		virtual void PostRender();
		virtual void SetEnableHighlight(bool enable);

		virtual IMaterial* GetMaterial(int pass = 0) const;

		// order of inserting meshes is important. parent first.
		// transform : parent space
		virtual size_t AddMesh(IMeshObject* mesh, const Transformation& transform, size_t parent);
		virtual const char* GetNameOfMesh(size_t idx);
		virtual size_t GetNumMeshes() const;

		virtual void AddMeshRotation(size_t idx, const Quat& rot);
		virtual const Quat& GetMeshRotation(size_t idx) const;
		virtual void SetMeshRotation(size_t idx, const Quat& rot);
		virtual const Vec3& GetMeshOffset(size_t idx) const;

		virtual const AUXILIARIES* GetAuxiliaries(size_t idx) const;
		virtual void SetAuxiliaries(size_t idx, const AUXILIARIES& aux);
		virtual void SetCollisionShapes(COLLISION_INFOS& colInfos);
		virtual void DeleteCollisionShapes();
		virtual void AddAuxiliary(size_t idx, const AUXILIARIES::value_type& v);
		virtual void AddCollisionShape(size_t idx, std::pair<FBColShape::Enum, Transformation>& data);
		virtual void SetCollisionMesh(size_t idx, IMeshObject* colMesh);
		virtual void UpdateTransform(bool force = false);
		
		virtual void SetAnimationData(const char* meshName, const AnimationData& anim, const char* actionName);

		virtual void PlayAction(const std::string& name, bool immediate, bool reverse);
		virtual bool IsActionDone(const char* action) const;

		virtual Transformation GetToLocalTransform(unsigned meshIdx);
		virtual Transformation GetToLocalTransform(const char* meshName);

		virtual Transformation GetLocalMeshTransform(unsigned meshIdx);

		virtual void SetAlpha(float alpha);
		virtual void SetForceAlphaBlending(bool enable, float alpha, float forceGlow = 0.f, bool disableDepth = false);
		virtual void SetAmbientColor(const Color& color);
		virtual IMeshObject* GetMeshObject(unsigned idx);

		virtual unsigned GetNumCollisionShapes() const;
		virtual const FBCollisionShape* GetCollisionShape(unsigned idx) const;
		virtual bool HasCollisionShapes() const {
			return mCollisionsCloned ? !mCollisionsCloned->empty() : false;
		}
		virtual unsigned GetNumCollisionShapes(unsigned idx) const;

		struct Hierarchy
		{
			size_t mParentIndex;
			size_t mMyIndex;
			std::vector<size_t> mChildren;
		};

		virtual bool IsPlayingAction() const;

	private:
		friend class Engine;
		virtual void Delete();

	private:
		typedef std::vector< std::pair< SmartPtr<IMeshObject>, Transformation> > MESH_OBJECTS;		
		MESH_OBJECTS mMeshObjects;
		typedef std::vector< Transformation > LOCAL_TRANSFORMATIONS;
		LOCAL_TRANSFORMATIONS mLocalTransforms;
		typedef std::vector< bool > CHANGES;
		CHANGES mChanges;
		typedef std::map<size_t, Hierarchy> HIERARCHY_MAP;
		HIERARCHY_MAP mHierarchyMap;

		typedef VectorMap< size_t, AUXILIARIES > AUXIL_MAP;
		AUXILIARIES mRootAuxil;
		AUXIL_MAP mAuxil;
		const AUXILIARIES* mRootAuxCloned;
		const AUXIL_MAP* mAuxCloned;
		Timer::FRAME_PRECISION mLastUpdateFrame;

		typedef std::vector< FBCollisionShape* > COLLISION_SHAPES;
		COLLISION_SHAPES mCollisions;
		COLLISION_SHAPES* mCollisionsCloned;

		bool mRootAnimated;

	};
}