#pragma once

template<typename T>
constexpr T smallEpsilon()
{
	return T(0.0000001);
}

template<typename T>
bool epsilonEqual(const T & a, const T & b)
{
	return abs(a - b) <= smallEpsilon<T>();
}

template<typename T, glm::precision P, template <typename, glm::precision> class vecType>
T lengthSquared(const vecType<T, P> & vec)
{
	return glm::dot(vec, vec);
}

template<typename T, glm::precision P>
glm::tquat<T, P> rotationBetweenNormalized(const glm::tvec3<T, P> & v1, const glm::tvec3<T, P> & v2)
{
	T dot = glm::dot(v1, v2);
	T limit = T(1) - smallEpsilon<T>();
	if (dot > limit)
	{
		return glm::tquat<T, P>();
	}
	else if (dot < -limit)
	{
		auto axis = glm::cross(v1, glm::tvec3<T, P>(1.f, 0.f, 0.f));
		if (lengthSquared(axis) < smallEpsilon<T>())
			axis = glm::cross(v1, glm::tvec3<T, P>(0.f, 1.f, 0.f));
		axis = glm::normalize(axis);
		return glm::angleAxis(glm::pi<T>(), axis);
	}
	else
	{
		return glm::normalize(glm::tquat<T, P>(T(1) + dot, glm::cross(v1, v2)));
	}
}

template<typename T, glm::precision P>
glm::tquat<T, P> rotationBetween(const glm::tvec3<T, P> & v1, const glm::tvec3<T, P> & v2)
{
	return rotationBetweenNormalized(glm::normalize(v1), glm::normalize(v2));
}

template<typename T, glm::precision P>
glm::tquat<T, P> rotateForwardToNormalized(const glm::tvec3<T, P> & v)
{
	return rotationBetweenNormalized(glm::tvec3<T, P>(T(0), T(0), T(-1)), v);
}

template<typename T, glm::precision P>
glm::tquat<T, P> rotateForwardTo(const glm::tvec3<T, P> & v)
{
	return rotateForwardToNormalized(glm::normalize(v));
}

template<typename T, glm::precision P>
void vrMatrixToGlm(glm::tmat4x4<T, P> & glmMat, const vr::HmdMatrix44_t & vrMat)
{
	glmMat = glm::tmat4x4<T, P>(
		vrMat.m[0][0], vrMat.m[1][0], vrMat.m[2][0], vrMat.m[3][0],
		vrMat.m[0][1], vrMat.m[1][1], vrMat.m[2][1], vrMat.m[3][1],
		vrMat.m[0][2], vrMat.m[1][2], vrMat.m[2][2], vrMat.m[3][2],
		vrMat.m[0][3], vrMat.m[1][3], vrMat.m[2][3], vrMat.m[3][3] );
}

template<typename T, glm::precision P>
void vrMatrixToGlm(glm::tmat4x4<T, P> & glmMat, const vr::HmdMatrix34_t & vrMat)
{
	glmMat = glm::tmat4x4<T, P>(
		vrMat.m[0][0], vrMat.m[1][0], vrMat.m[2][0], (T)0.0,
		vrMat.m[0][1], vrMat.m[1][1], vrMat.m[2][1], (T)0.0,
		vrMat.m[0][2], vrMat.m[1][2], vrMat.m[2][2], (T)0.0,
		vrMat.m[0][3], vrMat.m[1][3], vrMat.m[2][3], (T)1.0);
}
