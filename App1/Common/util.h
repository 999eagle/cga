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
