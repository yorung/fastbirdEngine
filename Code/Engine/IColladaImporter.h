#pragma once
#include <CommonLib/IteratorWrapper.h>
namespace fastbird
{
	class IMeshObject;
	class MeshGroup;
	class IColladaImporter : public ReferenceCounter
	{
	public:
		typedef std::map<std::string, SmartPtr<IMeshObject>> MeshObjects;

		static IColladaImporter* CreateColladaImporter();
		static void DeleteColladaImporter(IColladaImporter*);

		virtual ~IColladaImporter() {}
		// mergeMaterialGroups is used by voxelizer and modules.
		// if you use merging, you have to set material by yourself.
		virtual bool ImportCollada(const char* filepath, bool yzSwap_false, bool oppositeCull_true, 
			bool useIndexBuffer, bool mergeMaterialGroups, bool keepMeshData, bool generateTangent, bool meshGroup) = 0;
		virtual IteratorWrapper<MeshObjects> GetMeshIterator() = 0;
		virtual IMeshObject* GetMeshObject() const = 0;
		virtual IMeshObject* GetMeshObject(const char* id) const = 0;
		virtual IMeshGroup* GetMeshGroup() const = 0;

	};
}