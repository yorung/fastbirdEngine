#pragma once
namespace fastbird
{
	enum VERTEX_COMPONENT
	{
		VC_POSITION = 0x1,
		VC_NORMAL = 0x2,
		VC_COLOR =0x4,
		VC_TEXCOORD = 0x8,
		VC_TANGENT = 0x10,
		VC_BLENDINDICES = 0x20,
	};

	enum BUFFER_USAGE
	{
		BUFFER_USAGE_DEFAULT,
		BUFFER_USAGE_IMMUTABLE,
		BUFFER_USAGE_DYNAMIC,
		BUFFER_USAGE_STAGING,

		BUFFER_USAGE_COUNT
	};

	enum BIND_FLAG
	{
		BIND_VERTEX_BUFFER	= 0x1L,
		BIND_INDEX_BUFFER	= 0x2L,
		BIND_CONSTANT_BUFFER	= 0x4L,
		BIND_SHADER_RESOURCE	= 0x8L,
		BIND_STREAM_OUTPUT	= 0x10L,
		BIND_RENDER_TARGET	= 0x20L,
		BIND_DEPTH_STENCIL	= 0x40L,
		BIND_UNORDERED_ACCESS	= 0x80L
	};

	enum BUFFER_CPU_ACCESS_FLAG
	{
		BUFFER_CPU_ACCESS_NONE = 0,
		BUFFER_CPU_ACCESS_WRITE = 0x10000L,
		BUFFER_CPU_ACCESS_READ = 0x20000L
	};

	enum MAP_TYPE
	{
		MAP_TYPE_READ = 1,
		MAP_TYPE_WRITE = 2,
		MAP_TYPE_READ_WRITE = 3,
		MAP_TYPE_WRITE_DISCARD = 4,
		MAP_TYPE_WRITE_NO_OVERWRITE = 5
	};

	enum MAP_FLAG
	{
		MAP_FLAG_NONE = 0,
		MAP_FLAG_DO_NOT_WAIT = 0x100000L
	};

	enum PIXEL_FORMAT
	{
		PIXEL_FORMAT_UNKNOWN	                    = 0,
		PIXEL_FORMAT_R32G32B32A32_TYPELESS       = 1,
		PIXEL_FORMAT_R32G32B32A32_FLOAT          = 2,
		PIXEL_FORMAT_R32G32B32A32_UINT           = 3,
		PIXEL_FORMAT_R32G32B32A32_SINT           = 4,
		PIXEL_FORMAT_R32G32B32_TYPELESS          = 5,
		PIXEL_FORMAT_R32G32B32_FLOAT             = 6,
		PIXEL_FORMAT_R32G32B32_UINT              = 7,
		PIXEL_FORMAT_R32G32B32_SINT              = 8,
		PIXEL_FORMAT_R16G16B16A16_TYPELESS       = 9,
		PIXEL_FORMAT_R16G16B16A16_FLOAT          = 10,
		PIXEL_FORMAT_R16G16B16A16_UNORM          = 11,
		PIXEL_FORMAT_R16G16B16A16_UINT           = 12,
		PIXEL_FORMAT_R16G16B16A16_SNORM          = 13,
		PIXEL_FORMAT_R16G16B16A16_SINT           = 14,
		PIXEL_FORMAT_R32G32_TYPELESS             = 15,
		PIXEL_FORMAT_R32G32_FLOAT                = 16,
		PIXEL_FORMAT_R32G32_UINT                 = 17,
		PIXEL_FORMAT_R32G32_SINT                 = 18,
		PIXEL_FORMAT_R32G8X24_TYPELESS           = 19,
		PIXEL_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
		PIXEL_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
		PIXEL_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
		PIXEL_FORMAT_R10G10B10A2_TYPELESS        = 23,
		PIXEL_FORMAT_R10G10B10A2_UNORM           = 24,
		PIXEL_FORMAT_R10G10B10A2_UINT            = 25,
		PIXEL_FORMAT_R11G11B10_FLOAT             = 26,
		PIXEL_FORMAT_R8G8B8A8_TYPELESS           = 27,
		PIXEL_FORMAT_R8G8B8A8_UNORM              = 28,
		PIXEL_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
		PIXEL_FORMAT_R8G8B8A8_UINT               = 30,
		PIXEL_FORMAT_R8G8B8A8_SNORM              = 31,
		PIXEL_FORMAT_R8G8B8A8_SINT               = 32,
		PIXEL_FORMAT_R16G16_TYPELESS             = 33,
		PIXEL_FORMAT_R16G16_FLOAT                = 34,
		PIXEL_FORMAT_R16G16_UNORM                = 35,
		PIXEL_FORMAT_R16G16_UINT                 = 36,
		PIXEL_FORMAT_R16G16_SNORM                = 37,
		PIXEL_FORMAT_R16G16_SINT                 = 38,
		PIXEL_FORMAT_R32_TYPELESS                = 39,
		PIXEL_FORMAT_D32_FLOAT                   = 40,
		PIXEL_FORMAT_R32_FLOAT                   = 41,
		PIXEL_FORMAT_R32_UINT                    = 42,
		PIXEL_FORMAT_R32_SINT                    = 43,
		PIXEL_FORMAT_R24G8_TYPELESS              = 44,
		PIXEL_FORMAT_D24_UNORM_S8_UINT           = 45,
		PIXEL_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
		PIXEL_FORMAT_X24_TYPELESS_G8_UINT        = 47,
		PIXEL_FORMAT_R8G8_TYPELESS               = 48,
		PIXEL_FORMAT_R8G8_UNORM                  = 49,
		PIXEL_FORMAT_R8G8_UINT                   = 50,
		PIXEL_FORMAT_R8G8_SNORM                  = 51,
		PIXEL_FORMAT_R8G8_SINT                   = 52,
		PIXEL_FORMAT_R16_TYPELESS                = 53,
		PIXEL_FORMAT_R16_FLOAT                   = 54,
		PIXEL_FORMAT_D16_UNORM                   = 55,
		PIXEL_FORMAT_R16_UNORM                   = 56,
		PIXEL_FORMAT_R16_UINT                    = 57,
		PIXEL_FORMAT_R16_SNORM                   = 58,
		PIXEL_FORMAT_R16_SINT                    = 59,
		PIXEL_FORMAT_R8_TYPELESS                 = 60,
		PIXEL_FORMAT_R8_UNORM                    = 61,
		PIXEL_FORMAT_R8_UINT                     = 62,
		PIXEL_FORMAT_R8_SNORM                    = 63,
		PIXEL_FORMAT_R8_SINT                     = 64,
		PIXEL_FORMAT_A8_UNORM                    = 65,
		PIXEL_FORMAT_R1_UNORM                    = 66,
		PIXEL_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
		PIXEL_FORMAT_R8G8_B8G8_UNORM             = 68,
		PIXEL_FORMAT_G8R8_G8B8_UNORM             = 69,
		PIXEL_FORMAT_BC1_TYPELESS                = 70,
		PIXEL_FORMAT_BC1_UNORM                   = 71,
		PIXEL_FORMAT_BC1_UNORM_SRGB              = 72,
		PIXEL_FORMAT_BC2_TYPELESS                = 73,
		PIXEL_FORMAT_BC2_UNORM                   = 74,
		PIXEL_FORMAT_BC2_UNORM_SRGB              = 75,
		PIXEL_FORMAT_BC3_TYPELESS                = 76,
		PIXEL_FORMAT_BC3_UNORM                   = 77,
		PIXEL_FORMAT_BC3_UNORM_SRGB              = 78,
		PIXEL_FORMAT_BC4_TYPELESS                = 79,
		PIXEL_FORMAT_BC4_UNORM                   = 80,
		PIXEL_FORMAT_BC4_SNORM                   = 81,
		PIXEL_FORMAT_BC5_TYPELESS                = 82,
		PIXEL_FORMAT_BC5_UNORM                   = 83,
		PIXEL_FORMAT_BC5_SNORM                   = 84,
		PIXEL_FORMAT_B5G6R5_UNORM                = 85,
		PIXEL_FORMAT_B5G5R5A1_UNORM              = 86,
		PIXEL_FORMAT_B8G8R8A8_UNORM              = 87,
		PIXEL_FORMAT_B8G8R8X8_UNORM              = 88,
		PIXEL_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
		PIXEL_FORMAT_B8G8R8A8_TYPELESS           = 90,
		PIXEL_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
		PIXEL_FORMAT_B8G8R8X8_TYPELESS           = 92,
		PIXEL_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
		PIXEL_FORMAT_BC6H_TYPELESS               = 94,
		PIXEL_FORMAT_BC6H_UF16                   = 95,
		PIXEL_FORMAT_BC6H_SF16                   = 96,
		PIXEL_FORMAT_BC7_TYPELESS                = 97,
		PIXEL_FORMAT_BC7_UNORM                   = 98,
		PIXEL_FORMAT_BC7_UNORM_SRGB              = 99,
		PIXEL_FORMAT_FORCE_UINT                  = 0xffffffff
	};

	enum BINDING_SHADER
	{
		BINDING_SHADER_VS = 0x1,
		BINDING_SHADER_HS = 0x2,
		BINDING_SHADER_DS = 0x4,		
		BINDING_SHADER_GS = 0x8,
		BINDING_SHADER_PS = 0x10,
		BINDING_SHADER_CS = 0x20,
		BINDING_SHADER_COUNT
	};

	BINDING_SHADER BindingShaderFromString(const char* szShader);

	enum FILL_MODE
	{
		FILL_MODE_WIREFRAME=2,
		FILL_MODE_SOLID=3,
	};
	FILL_MODE FillModeFromString(const char* str);

	enum CULL_MODE
	{
		CULL_MODE_NONE=1,
		CULL_MODE_FRONT=2,
		CULL_MODE_BACK=3
	};
	CULL_MODE CullModeFromString(const char* str);

	enum TEXTURE_TYPE
	{
		TEXTURE_TYPE_DEFAULT=0x1,
		TEXTURE_TYPE_COLOR_RAMP=0x2,
		TEXTURE_TYPE_RENDER_TARGET=0x4,
		TEXTURE_TYPE_RENDER_TARGET_SRV=0x8,
		TEXTURE_TYPE_DEPTH_STENCIL=0x10,
		TEXTURE_TYPE_DEPTH_STENCIL_SRV=0x20, //DXGI_FORMAT_R32_TYPELESS
		TEXTURE_TYPE_CUBE_MAP=0x40,
		TEXTURE_TYPE_MULTISAMPLE = 0x80,
		TEXTURE_TYPE_MIPS = 0x100,
		TEXTURE_TYPE_COUNT
	};

	TEXTURE_TYPE TextureTypeFromString(const char* str);
	
	enum TEXTURE_FILTER
	{
		TEXTURE_FILTER_MIN_MAG_MIP_POINT	= 0,
		TEXTURE_FILTER_MIN_MAG_POINT_MIP_LINEAR	= 0x1,
		TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x4,
		TEXTURE_FILTER_MIN_POINT_MAG_MIP_LINEAR	= 0x5,
		TEXTURE_FILTER_MIN_LINEAR_MAG_MIP_POINT	= 0x10,
		TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x11,
		TEXTURE_FILTER_MIN_MAG_LINEAR_MIP_POINT	= 0x14,
		TEXTURE_FILTER_MIN_MAG_MIP_LINEAR	= 0x15,
		TEXTURE_FILTER_ANISOTROPIC	= 0x55,
		TEXTURE_FILTER_COMPARISON_MIN_MAG_MIP_POINT	= 0x80,
		TEXTURE_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR	= 0x81,
		TEXTURE_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x84,
		TEXTURE_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR	= 0x85,
		TEXTURE_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT	= 0x90,
		TEXTURE_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x91,
		TEXTURE_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT	= 0x94,
		TEXTURE_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR	= 0x95,
		TEXTURE_FILTER_COMPARISON_ANISOTROPIC	= 0xd5
	};

	TEXTURE_FILTER FilterFromString(const char* sz);

	enum TEXTURE_ADDRESS_MODE
	{
		TEXTURE_ADDRESS_WRAP	= 1,
		TEXTURE_ADDRESS_MIRROR	= 2,
		TEXTURE_ADDRESS_CLAMP	= 3,
		TEXTURE_ADDRESS_BORDER	= 4,
		TEXTURE_ADDRESS_MIRROR_ONCE	= 5
	};

	TEXTURE_ADDRESS_MODE AddressModeFromString(const char* sz);

	enum COMPARISON_FUNC
	{
		COMPARISON_NEVER			= 1,
		COMPARISON_LESS				= 2,
		COMPARISON_EQUAL			= 3,
		COMPARISON_LESS_EQUAL		= 4,
		COMPARISON_GREATER			= 5,
		COMPARISON_NOT_EQUAL		= 6,
		COMPARISON_GREATER_EQUAL	= 7,
		COMPARISON_ALWAYS			= 8
	};

	static const char* STR_COMPARISON_FUNC[]=
	{
		"COMPARISON_NEVER",
		"COMPARISON_LESS",
		"COMPARISON_EQUAL",
		"COMPARISON_LESS_EQUAL",
		"COMPARISON_GREATER",
		"COMPARISON_NOT_EQUAL",
		"COMPARISON_GREATER_EQUAL",
		"COMPARISON_ALWAYS",
	};

	COMPARISON_FUNC ComparisonFuncFromString(const char* str);

	enum BLEND
	{
		BLEND_ZERO               = 1,
		BLEND_ONE                = 2,
		BLEND_SRC_COLOR          = 3,
		BLEND_INV_SRC_COLOR      = 4,
		BLEND_SRC_ALPHA          = 5,
		BLEND_INV_SRC_ALPHA      = 6,
		BLEND_DEST_ALPHA         = 7,
		BLEND_INV_DEST_ALPHA     = 8,
		BLEND_DEST_COLOR         = 9,
		BLEND_INV_DEST_COLOR     = 10,
		BLEND_SRC_ALPHA_SAT      = 11,
		BLEND_INVALID1			 = 12,
		BLEND_INVALID2			 = 13,
		BLEND_BLEND_FACTOR       = 14,
		BLEND_INV_BLEND_FACTOR   = 15,
		BLEND_SRC1_COLOR         = 16,
		BLEND_INV_SRC1_COLOR     = 17,
		BLEND_SRC1_ALPHA         = 18,
		BLEND_INV_SRC1_ALPHA     = 19 
	};

	static const char* STR_BLEND[] =
	{
		"BLEND_ZERO",
		"BLEND_ONE",
		"BLEND_SRC_COLOR",
		"BLEND_INV_SRC_COLOR",
		"BLEND_SRC_ALPHA",
		"BLEND_INV_SRC_ALPHA",
		"BLEND_DEST_ALPHA",
		"BLEND_INV_DEST_ALPHA",
		"BLEND_DEST_COLOR",
		"BLEND_INV_DEST_COLOR",
		"BLEND_SRC_ALPHA_SAT",
		"BLEND_INVALID1",
		"BLEND_INVALID2",
		"BLEND_BLEND_FACTOR",
		"BLEND_INV_BLEND_FACTOR",
		"BLEND_SRC1_COLOR",
		"BLEND_INV_SRC1_COLOR",
		"BLEND_SRC1_ALPHA",
		"BLEND_INV_SRC1_ALPHA",
	};

	BLEND BlendFromString(const char* str);

	enum BLEND_OP
	{
		BLEND_OP_ADD            = 1,
		BLEND_OP_SUBTRACT       = 2,
		BLEND_OP_REV_SUBTRACT   = 3,
		BLEND_OP_MIN            = 4,
		BLEND_OP_MAX            = 5
	};

	static const char* STR_BLEND_OP[] = {
		"BLEND_OP_ADD",
		"BLEND_OP_SUBTRACT",
		"BLEND_OP_REV_SUBTRACT",
		"BLEND_OP_MIN",
		"BLEND_OP_MAX",
	};

	BLEND_OP BlendOpFromString(const char* str);

	enum COLOR_WRITE_MASK
    {
		COLOR_WRITE_MASK_NONE = 0,
		COLOR_WRITE_MASK_RED		= 1,
		COLOR_WRITE_MASK_GREEN		= 2,
		COLOR_WRITE_MASK_BLUE		= 4,
		COLOR_WRITE_MASK_ALPHA		= 8,
		COLOR_WRITE_MASK_ALL	= ( ( ( COLOR_WRITE_MASK_RED | COLOR_WRITE_MASK_GREEN )  | COLOR_WRITE_MASK_BLUE )  | COLOR_WRITE_MASK_ALPHA ) 
    };

	COLOR_WRITE_MASK ColorWriteMaskFromString(const char* str);

	enum PRIMITIVE_TOPOLOGY
	{
		PRIMITIVE_TOPOLOGY_UNKNOWN,
		PRIMITIVE_TOPOLOGY_POINTLIST,
		PRIMITIVE_TOPOLOGY_LINELIST,
		PRIMITIVE_TOPOLOGY_LINESTRIP,
		PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	};

	enum INPUT_ELEMENT_FORMAT
	{
		INPUT_ELEMENT_FORMAT_FLOAT4,
		INPUT_ELEMENT_FORMAT_FLOAT3,
		INPUT_ELEMENT_FORMAT_UBYTE4,
		INPUT_ELEMENT_FORMAT_FLOAT2,
		INPUT_ELEMET_FORMAT_INT4,

		INPUT_ELEMET_FORMAT_NUM
	};

	static const char* STR_INPUT_ELEMENT_FORMAT[] =
	{
		"FLOAT4",
		"FLOAT3",
		"UBYTE4",
		"FLOAT2",
		"INT4"
	};

	INPUT_ELEMENT_FORMAT InputElementFromString(const char* sz);

	enum INPUT_CLASSIFICATION
	{
		INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		INPUT_CLASSIFICATION_PER_INSTANCE_DATA,

		INPUT_CLASSIFICATION_NUM
	};
	static const char* STR_INPUT_CLASSIFICATION[] = 
	{
		"VERTEX",
		"INSTANCE",
	};
	INPUT_CLASSIFICATION InputClassificationFromString(const char* sz);

	enum DEPTH_WRITE_MASK
	{
		DEPTH_WRITE_MASK_ZERO 	=0,
		DEPTH_WRITE_MASK_ALL  	=1,
	};

	DEPTH_WRITE_MASK DepthWriteMaskFromString(const char* str);

	enum STENCIL_OP
    {	
		STENCIL_OP_KEEP			= 1,
		STENCIL_OP_ZERO			= 2,
		STENCIL_OP_REPLACE		= 3,
		STENCIL_OP_INCR_SAT		= 4,
		STENCIL_OP_DECR_SAT		= 5,
		STENCIL_OP_INVERT		= 6,
		STENCIL_OP_INCR			= 7,
		STENCIL_OP_DECR			= 8
	};
	static const char* STR_STENCIL_OP[] = {
		"STENCIL_OP_KEEP",
		"STENCIL_OP_ZERO",
		"STENCIL_OP_REPLACE",
		"STENCIL_OP_INCR_SAT",
		"STENCIL_OP_DECR_SAT",
		"STENCIL_OP_INVERT",
		"STENCIL_OP_INCR",
		"STENCIL_OP_DECR",
	};
	STENCIL_OP StencilOpFromString(const char* str);
	
	enum RENDER_PASS
	{
		PASS_NORMAL = 0,
		PASS_GODRAY_OCC_PRE, // for GodRay - occlusion pre pass
		PASS_GLOW,
		PASS_DEPTH, // write depth to render target
		PASS_DEPTH_ONLY, // write depth only to the depth buffer.
		PASS_SHADOW,
		PASS_SILOUETTE,
	};

	RENDER_PASS RenderPassFromString(const char* str);

	namespace SAMPLERS
	{
		enum Enum{
			POINT,
			LINEAR,
			ANISOTROPIC,
			SHADOW,
			POINT_WRAP,
			LINEAR_WRAP,
			BLACK_BORDER,

			NUM
		};
	}
}