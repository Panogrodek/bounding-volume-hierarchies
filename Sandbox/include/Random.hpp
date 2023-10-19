#pragma once

#include <random>

static std::mt19937 m_randEngine;

template<typename T>
static T Rand(T min, T max) {
	return std::uniform_real_distribution<T>(min, max)(m_randEngine);
}