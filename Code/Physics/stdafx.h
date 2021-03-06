#include <CommonLib/Config.h>

#ifdef _DEBUG
#pragma comment(lib, "CommonLib_Debug.lib")
#else
#pragma comment(lib, "CommonLib_Release.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "BulletCollision_vs2010_debug.lib")
#pragma comment(lib, "BulletDynamics_vs2010_debug.lib")
#pragma comment(lib, "LinearMath_vs2010_debug.lib")
#else
#pragma comment(lib, "BulletCollision_vs2010.lib")
#pragma comment(lib, "BulletDynamics_vs2010.lib")
#pragma comment(lib, "LinearMath_vs2010.lib")
#endif

#include <iostream>

#include <CommonLib/CommonLib.h>
#include <CommonLib/SmartPtr.h>
#include <CommonLib/Math/Vec2.h>
#include <CommonLib/Math/Vec2I.h>
#include <CommonLib/Math/fbMath.h>
#include <CommonLib/VectorMap.h>
#include <CommonLib/StringUtils.h>
#include <CommonLib/tinyxml2.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btTransform.h>

#include <Physics/mathConv.h>


#define DEFAULT_DEBUG_ARG "%s(%d): %s() - %s", __FILE__, __LINE__, __FUNCTION__