#pragma once

#include <Engine/IMeshObject.h>
#include <../es/shaders/Constants.h>

namespace fastbird
{
	class FBCollisionShape;
	class MeshObject : public IMeshObject
	{
	public:
		MeshObject();
		virtual ~MeshObject();
		
		//------------------------------------------------------------------------
		// IObject
		//------------------------------------------------------------------------
		virtual void PreRender();
		virtual void Render();		
		virtual void PostRender();
		virtual void SetMaterial(const char* name, int pass = 0);
		virtual void SetMaterial(IMaterial* pMat, int pass = 0);
		virtual void SetMaterialFor(int matGroupIdx, IMaterial* pMat);
		virtual IMaterial* GetMaterial(int pass =0) const;
		virtual void SetName(const char* name) { mName = name;}
		virtual const char* GetName() const { return mName.c_str(); }
		virtual void SetEnableHighlight(bool enable);

		//------------------------------------------------------------------------
		// IMeshObject
		//------------------------------------------------------------------------
		virtual void RenderSimple();
		virtual IObject* Clone() const;
		virtual bool LoadOgreMesh(const char* filename);
		virtual void ClearMeshData();

		virtual void StartModification();
		virtual void AddTriangle(int matGroupIdx, const Vec3& pos0, const Vec3& pos1, const Vec3& pos2);
		virtual void AddQuad(int matGroupIdx, const Vec3 pos[4], const Vec3 normal[4]);
		virtual void AddQuad(int matGroupIdx, const Vec3 pos[4], const Vec3 normal[4], const Vec2 uv[4]);
		virtual void SetPositions(int matGroupIdx, const Vec3* p, size_t numVertices);
		virtual void SetNormals(int matGroupIdx, const Vec3* n, size_t numNormals);
		virtual void SetUVs(int matGroupIdx, const Vec2* uvs, size_t numUVs);
		virtual void SetTriangles(int matGroupIdx, const ModelTriangle* tris, size_t numTri);
		virtual void SetColors(int matGroupIdx, const DWORD* colors, size_t numColors);
		virtual void SetTangents(int matGroupIdx, const Vec3* t, size_t numTangents);
		virtual void SetIndices(int matGroupIdx, const UINT* indices, size_t numIndices);
		virtual void SetIndices(int matGroupIdx, const USHORT* indices, size_t numIndices);
		virtual void SetIndexBuffer(int matGroupIdx, IIndexBuffer* pIndexBuffer);
		virtual Vec3* GetPositions(int matGroupIdx, size_t& outNumPositions);
		virtual Vec3* GetNormals(int matGroupIdx, size_t& outNumNormals);
		virtual Vec2* GetUVs(int matGroupIdx, size_t& outNumUVs);
		virtual void GenerateTangent(int matGroupIdx, UINT* indices, size_t num);
		virtual void EndModification(bool keepMeshData);

		virtual void SetTopology(PRIMITIVE_TOPOLOGY topology);
		virtual PRIMITIVE_TOPOLOGY GetTopology();
		virtual void SetInstanceVB(IVertexBuffer* pBuffer);

		virtual const AUXILIARIES& GetAuxiliaries() const { return mAuxCloned ? *mAuxCloned : mAuxil; }
		virtual void SetAuxiliaries(const AUXILIARIES& aux) {mAuxil = aux; }
		virtual void AddCollisionShape(const COL_SHAPE& data);
		virtual void SetCollisionShapes(COLLISION_INFOS& colInfos);
		virtual void SetCollisionShapes(std::vector< COL_SHAPE >& shapes);
		virtual void SetCollisionMesh(IMeshObject* colMesh);
		virtual unsigned GetNumCollisionShapes() const { return mCollisionsCloned ? mCollisionsCloned->size() : 0; }
		virtual bool HasCollisionShapes() const {
			return mCollisionsCloned ? !mCollisionsCloned->empty() : false;
		}
		virtual const FBCollisionShape* GetCollisionShape(unsigned idx) const { return mCollisionsCloned ? (*mCollisionsCloned)[idx] : 0; }
		virtual bool CheckNarrowCollision(fastbird::BoundingVolume* pBV) const;
		virtual Ray3::IResult CheckNarrowCollisionRay(const Ray3& ray) const;
		virtual Vec3 GetRandomPosInVolume(const Vec3* nearWorld = 0) const;
		void DeleteCollisionShapes();
		virtual void SetUseDynamicVB(BUFFER_TYPE type,  bool useDynamicVB);
		virtual MapData MapVB(BUFFER_TYPE type, size_t materialGroupIdx);
		virtual void UnmapVB(BUFFER_TYPE type, size_t materialGroupIdx);
		virtual bool RayCast(const Ray3& ray, Vec3& location, const ModelTriangle** outTri=0) const;
		virtual void MakeMaterialIndependent();
		virtual BoundingVolume* GetAABB() const { return mAABB; }
		virtual bool IsPlayingAction() const;
		virtual void ClearVertexBuffers();

		struct MaterialGroup
		{
			SmartPtr<IMaterial> mMaterial;
			SmartPtr<IMaterial> mForceAlphaMaterial;
			SmartPtr<IVertexBuffer> mVBPos;
			SmartPtr<IVertexBuffer> mVBNormal;
			SmartPtr<IVertexBuffer> mVBUV;
			SmartPtr<IVertexBuffer> mVBColor;
			SmartPtr<IVertexBuffer> mVBTangent;
			
			SmartPtr<IIndexBuffer> mIndexBuffer;
			std::vector<Vec3> mPositions;
			std::vector<Vec3> mNormals;
			std::vector<Vec2> mUVs;	
			std::vector<ModelTriangle> mTriangles;
			std::vector<DWORD> mColors;
			std::vector<Vec3> mTangents;
		};

		virtual void SetAlpha(float alpha);
		virtual void SetForceAlphaBlending(bool enable, float alpha, float forceGlow = 0.f, bool disableDepth = false);
		virtual void SetAmbientColor(const Color& color);

	private:
		void CreateMaterialGroupFor(int matGroupIdx);
		friend class Engine;
		virtual void Delete();
		void RenderMaterialGroup(MaterialGroup* it, bool onlyPos);

	private:
		// OBJECT
		SmartPtr<IInputLayout> mInputLayoutOverride;
		std::string mName;

		PRIMITIVE_TOPOLOGY mTopology;
		OBJECT_CONSTANTS mObjectConstants;
		POINT_LIGHT_CONSTANTS mPointLightConstants;
		// if you have only one MaterialGroup,
		// this vector will not be used.
		std::vector<MaterialGroup> mMaterialGroups; // can call it as SubMeshes.
		
		AUXILIARIES mAuxil;
		AUXILIARIES* mAuxCloned;
		bool mModifying;
		bool mRenderHighlight;
		SmartPtr<IRasterizerState> mHighlightRasterizeState;

		typedef std::vector< FBCollisionShape* > COLLISION_SHAPES;
		COLLISION_SHAPES mCollisions;
		COLLISION_SHAPES* mCollisionsCloned;
		SmartPtr<BoundingVolume> mAABB;

		bool mUseDynamicVB[BUFFER_TYPE_NUM];

		bool mForceAlphaBlending;
	};
}