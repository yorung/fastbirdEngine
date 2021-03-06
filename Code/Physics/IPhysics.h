#pragma once

#include <Physics/RayResult.h>
class btTypedConstraint;
class btCollisionShape;
namespace fastbird
{
	class IDebugDrawer;
	class IWinBase;
	class RigidBody;
	class IPhysicsInterface;
	struct CollisionShape;
	struct BoxShape;
	struct SphereShape;
	struct CylinderShape;
	struct CapsuleShape;
	struct MeshShape;
	struct IFilterCallback;
	typedef bool(*NeedCollisionForConvexCallback)(RigidBody* a, RigidBody* b);
	class IPhysics
	{
	public:
		static IPhysics* GetPhysics();
		static const size_t MemAlign = 16;

		virtual ~IPhysics(){}

		//virtual void Initilaize() = 0;
		virtual void Deinitilaize() = 0;
		virtual void Update(float dt) = 0;

		//virtual void CreateRigidBody(float mass, const Transformation& startTransform, 
			//btCollisionShape)

		virtual RigidBody* CreateRigidBody(const char* collisionFile, float mass, IPhysicsInterface* obj) = 0;
		virtual RigidBody* CreateRigidBody(IPhysicsInterface* colProvider) = 0;
		virtual RigidBody* CreateRigidBodyForGroup(IPhysicsInterface* colProvider, const Vec3I& groupIdx) = 0;
		virtual RigidBody* CreateTempRigidBody(CollisionShape* colShape) = 0;
		virtual RigidBody* CreateTempRigidBody(CollisionShape*  shapes[], unsigned num) = 0;
		virtual void DeleteRigidBody(RigidBody* rigidBody) = 0;
		virtual void AddRef(btCollisionShape* colShape) = 0;
		virtual void Release(btCollisionShape* colShape) = 0;

		virtual void RemoveConstraint(btTypedConstraint* constraint) = 0;
		virtual void SetDebugDrawer(IDebugDrawer* debugDrawer) = 0;
		virtual void SetDebugMode(int debugMode) = 0;

		virtual void AttachBodies(RigidBody* bodies[], unsigned num) = 0;
		virtual void AttachBodiesAlways(RigidBody* bodies[], unsigned num) = 0;
		
		virtual void SetRayCollisionGroup(int group) = 0;
		virtual bool RayTestClosest(const Vec3& fromWorld, const Vec3& toWorld, int additionalGroupFlag, int mask, RayResultClosest& result, void* excepts[] = 0, unsigned numExcepts = 0) = 0;
		virtual bool RayTestWithAnObj(const Vec3& fromWorld, const Vec3& toWorld, int additionalGroupFlag, RayResultWithObj& result) = 0;
		// returning RayResultAll*
		// need to delete with IPhysics::Release(RayResultAll*)
		virtual RayResultAll* RayTestAll(const Vec3& fromWorld, const Vec3& toWorld, int additionalGroupFlag, int mask) = 0;

		virtual void Release(RayResultAll* r) = 0;

		virtual unsigned GetAABBOverlaps(const AABB& aabb, unsigned colMask, RigidBody* ret[], unsigned limit, RigidBody* except) = 0;

		virtual float GetDistanceBetween(RigidBody* a, RigidBody* b) = 0;

		virtual unsigned CreateBTSphereShape(float radius) = 0;
		virtual void DeleteBTShape(unsigned id) = 0;

		//-------------------------------------------------------------------
		// collision shape manager
		//-------------------------------------------------------------------
		virtual BoxShape* CreateBoxShape(const Vec3& pos, const Quat& rot, const Vec3& actorScale, const Vec3& extent, void* userPtr = 0) = 0;
		virtual SphereShape* CreateSphereShape(const Vec3& pos, const Quat& rot, const Vec3& actorScale, float radius, void* userPtr = 0) = 0;
		virtual CylinderShape* CreateCylinderShape(const Vec3& pos, const Quat& rot, const Vec3& actorScale, const Vec3& extent, void* userPtr = 0) = 0;
		virtual CapsuleShape* CreateCylinderShape(const Vec3& pos, const Quat& rot, const Vec3& actorScale, float radius, float height, void* userPtr = 0) = 0;
		virtual MeshShape* CreateMeshShape(const Vec3& pos, const Quat& rot, Vec3* vertices, unsigned numVertices, const Vec3& scale,
			bool staticObj, void* userPtr = 0) = 0;
		virtual MeshShape* CreateConvexMeshShape(const Vec3& pos, const Quat& rot, Vec3* vertices, unsigned numVertices,
			const Vec3& scale, void* userPtr = 0) = 0;
		virtual void DestroyShape(CollisionShape* shape) = 0;


		virtual void RegisterFilterCallback(IFilterCallback* callback, NeedCollisionForConvexCallback func) = 0;
	};
}

extern fastbird::IPhysics* gFBPhysics;