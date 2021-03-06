#include <CommonLib/StdAfx.h>
#include <CommonLib/Math/BVaabb.h>
#include <CommonLib/Math/Transformation.h>

namespace fastbird
{
BVaabb::BVaabb()
	: mCenter(0, 0, 0)
{
}

void BVaabb::Merge(const BoundingVolume* pBV)
{
	assert(pBV);
	const Vec3& center = pBV->GetCenter();
	float radius = pBV->GetRadius();
	mAABB.Merge(center + Vec3(radius, 0, 0));
	mAABB.Merge(center + Vec3(-radius, 0, 0));

	mAABB.Merge(center + Vec3(0, radius, 0));
	mAABB.Merge(center + Vec3(0, -radius, 0));

	mAABB.Merge(center + Vec3(0, 0, radius));
	mAABB.Merge(center + Vec3(0, 0, -radius));	
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length()*.5f;

}

void BVaabb::Merge(const Vec3& worldPos)
{
	mAABB.Merge(worldPos);
	mCenter = mAABB.GetCenter();
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length()*.5f;
}

void BVaabb::SetCenter (const Vec3& center)
{
	if (mAABB.IsValid())
	{
		Vec3 diff = center - mCenter;
		mAABB.Translate(diff);
		
	}
	else
	{
		mAABB.Merge(center);
	}
	mCenter = center;
	
}

void BVaabb::SetRadius (float fRadius)
{
	Vec3 center = mAABB.GetCenter();
	mAABB.Merge(center + Vec3(fRadius, 0, 0));
	mAABB.Merge(center + Vec3(-fRadius, 0, 0));

	mAABB.Merge(center + Vec3(0, fRadius, 0));
	mAABB.Merge(center + Vec3(0, -fRadius, 0));

	mAABB.Merge(center + Vec3(0, 0, fRadius));
	mAABB.Merge(center + Vec3(0, 0, -fRadius));	
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length() * .5f;
	
}

const Vec3& BVaabb::GetCenter () const
{
	return mCenter;
}

float BVaabb::GetRadius () const
{
	return mRadius;
}

void BVaabb::ComputeFromData(const Vec3* pVertices, size_t numVertices)
{
	assert(pVertices && numVertices>0);
	mAABB.Invalidate();
	for (size_t i=0; i<numVertices; i++)
	{
		mAABB.Merge(pVertices[i]);
	}
	mCenter = mAABB.GetCenter();
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length() * .5f;
}

void BVaabb::StartComputeFromData()
{
	mAABB.Invalidate();
}

void BVaabb::AddComputeData(const Vec3* pVertices, size_t numVertices)
{
	assert(pVertices && numVertices>0);
	for (size_t i=0; i<numVertices; i++)
	{
		mAABB.Merge(pVertices[i]);
	}
}

void BVaabb::AddComputeData(const Vec3& vert)
{
	mAABB.Merge(vert);
}

void BVaabb::EndComputeFromData()
{
	mCenter = mAABB.GetCenter();
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length() * .5f;
}

void BVaabb::TransformBy(const Transformation& transform,
	BoundingVolume* result)
{
	assert(result);
	BVaabb* pNewBound = (BVaabb*)result;
	AABB newAABB = mAABB;
	newAABB.Translate(transform.GetTranslation());
	pNewBound->SetAABB(newAABB);
}

int BVaabb::WhichSide(const Plane3& plane) const
{
	if (mAlwaysPass)
		return 1;
	float fDistance = plane.DistanceTo(mCenter);
	if (fDistance <= -mRadius)
	{
		return -1;
	}

	if (fDistance >= mRadius)
	{
		return +1;
	}

	return 0;
}

bool BVaabb::TestIntersection(const Ray3& ray) const
{
	Vec3 normal;
	Ray3::IResult ret = ray.intersects(mAABB, normal);
	return ret.first;
}


bool BVaabb::TestIntersection(BoundingVolume* pBV) const
{
	float R = pBV->GetRadius();
	const auto& S = pBV->GetCenter();
	float dist_squared = R * R;
	const auto& C1 = mAABB.GetMin();
	const auto& C2 = mAABB.GetMax();	
	if (S.x < C1.x) dist_squared -= Squared(S.x - C1.x);
	else if (S.x > C2.x) dist_squared -= Squared(S.x - C2.x);
	if (S.y < C1.y) dist_squared -= Squared(S.y - C1.y);
	else if (S.y > C2.y) dist_squared -= Squared(S.y - C2.y);
	if (S.z < C1.z) dist_squared -= Squared(S.z - C1.z);
	else if (S.z > C2.z) dist_squared -= Squared(S.z - C2.z);
	return dist_squared > 0;
}

void BVaabb::SetAABB(const AABB& aabb)
{
	mAABB = aabb;
	mCenter = mAABB.GetCenter();
	mRadius = (mAABB.GetMax() - mAABB.GetMin()).Length() * .5f;
}

BoundingVolume& BVaabb::operator= (const BoundingVolume& other)
{
	mAABB.Invalidate();
	mAABB.Merge(other.GetCenter());
	float radius = other.GetRadius();
	SetRadius(radius);
	mCenter = mAABB.GetCenter();
	return *this;
}

Vec3 BVaabb::GetSurfaceFrom(const Vec3& src, Vec3& normal)
{
	Vec3 dir = mCenter - src;
	dir.Normalize();
	Ray3 ray(src, dir);
	Ray3::IResult ret = ray.intersects(mAABB, normal);
	return dir * ret.second;
}

void BVaabb::Expand(float e)
{
	mAABB.SetMax(mAABB.GetMax() + Vec3(e));
	mAABB.SetMin(mAABB.GetMin() - Vec3(e));
	
}

bool BVaabb::Contain(const Vec3& pos) const
{
	return mAABB.Contain(pos);
}

Vec3 BVaabb::GetRandomPosInVolume(const Vec3* nearLocal) const
{
	const auto& mn = mAABB.GetMin();
	const auto& mx = mAABB.GetMax();
	auto pos = Random(mAABB.GetMin(), mAABB.GetMax());
	if (nearLocal)
	{
		pos = Lerp(pos, *nearLocal, Random(0.5f, 1.0f));
	}
	Vec3 dir = (pos - mAABB.GetCenter()).NormalizeCopy();
	Ray3 ray(pos, -dir);
	Vec3 normal;
	Ray3::IResult ret = ray.intersects(mAABB, normal);
	if (ret.first)
	{
		if (mAABB.Contain(pos))
		{
			return pos + dir * ret.second;
		}
		else
		{
			return pos - dir * ret.second;
		}
		
	}
	else
	{
		return pos;
	}

}

}
