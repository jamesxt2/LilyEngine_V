#pragma once
#include <iostream>
#include <memory>
#include <utility>

// GLM block
#define GLM_FORCE_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// GLFW block
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace LEV {
	template<class T>
	using Ref = std::shared_ptr<T>;

	template<class T>
	using Scope = std::unique_ptr<T>;

	template<class T>
	using WeakRef = std::weak_ptr<T>;

	template<class T, class...Args>
	[[nodiscard]] inline Ref<T> MakeRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
} // namespace LLV