#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include <type_traits>
#include <concepts>
#include "boost/pfr/pfr.hpp"
#include <string_view>
#include <iostream>
#include <array>
#include "OpenGLObjects/BUF.h"
#include "glad/glad.h"
#include <cassert>
#include "glm/glm.hpp"
#include <vector>
#include <span>

// Check if array
template <typename T>
struct is_array_like : std::false_type {};

template <typename T, typename A>
struct is_array_like<std::vector<T, A>> : std::true_type {};

template <typename T, int N>
struct is_array_like<std::array<T, N>> : std::true_type {};

template <typename T>
inline constexpr bool is_array_like_v = is_array_like<T>::value;

// Check if glm object
template <typename T>
struct is_glm : std::false_type {};

template<>
struct is_glm<glm::vec2> : std::true_type {};

template<>
struct is_glm<glm::vec3> : std::true_type {};

template<>
struct is_glm<glm::vec4> : std::true_type {};

template<>
struct is_glm<glm::mat4> : std::true_type {};

template <typename T>
inline constexpr bool is_glm_v = is_glm<T>::value;

// Concepts
template <typename T>
concept IsClassOrStruct = std::is_class_v<T> && !is_glm_v<T> && !is_array_like_v<T>;

template <typename T>
concept IsArrayLike = is_array_like_v<T>;

namespace BufferTypes {
	enum Type {
		uniform,
		ssbo,
		max
	};

	inline std::array<GLenum, (int)Type::max> bufferTargets{ {
		GL_UNIFORM_BUFFER,
		GL_SHADER_STORAGE_BUFFER
	} };
}

// Buffer helpers TODO clean up
namespace {
	inline int numFloats(int numBytes) {
		return numBytes / 4;
	}

	template <typename>
	inline constexpr bool always_false_v = false;

	template <typename T>
	inline int getBaseAlignment(BufferTypes::Type bufferType) {
		if constexpr (std::same_as<T, int>)
			return 4;
		else if constexpr (std::same_as<T, bool>)
			return 4;
		else if constexpr (std::same_as<T, float>)
			return 4;
		else if constexpr (std::same_as<T, glm::vec2>)
			return 8;
		else if constexpr (std::same_as<T, glm::vec3>)
			return 16;
		else if constexpr (std::same_as<T, glm::vec4>)
			return 16;
		else if constexpr (std::same_as<T, glm::mat4>)
			return 16;
		else if constexpr (IsClassOrStruct<T>)
			return 16;
		else if constexpr (IsArrayLike<T>)
			return bufferType == BufferTypes::uniform ? 16 : getBaseAlignment<typename T::value_type>(bufferType);
		else
			static_assert(always_false_v<T>, "getBaseAlignment<T> requires T to be one of int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4, std::vector, std::array, any struct");
	}

	template <typename T>
	inline int getNumOfFloatsInObject() {
		if constexpr (std::same_as<T, int>)
			return 1;
		else if constexpr (std::same_as<T, bool>)
			return 1;
		else if constexpr (std::same_as<T, float>)
			return 1;
		else if constexpr (std::same_as<T, glm::vec2>)
			return 2;
		else if constexpr (std::same_as<T, glm::vec3>)
			return 3;
		else if constexpr (std::same_as<T, glm::vec4>)
			return 4;
		else if constexpr (std::same_as<T, glm::mat4>)
			return 16;
		else
			static_assert(always_false_v<T>, "getNumOfFloatsInObject<T> requires T to be one of int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4");
	}
}

// The buffer class
// T must only contain int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4, and may have an std::vector as the last member if bufferType == ssbo
template <IsClassOrStruct BaseStructType>
class OpenGLBuffer {
public:

	OpenGLBuffer(int bindingIndex, BufferTypes::Type bufferType)
		: mBindingIndex{ bindingIndex }
		, mBufferType{ bufferType }
	{
		mBuffer.bind(BufferTypes::bufferTargets[(int)bufferType]);
		glBindBufferBase(BufferTypes::bufferTargets[(int)mBufferType], mBindingIndex, mBuffer);
	}

	bool updateGPU() {
		if (!mHasUpdatedGPU || mPrevValue != mValue) {
			std::vector<float> data;
			int baseOffset{ 0 };
			addStructToData(mValue, data, baseOffset, false);

			mBuffer.bind(BufferTypes::bufferTargets[(int)mBufferType]);
			glBufferData(BufferTypes::bufferTargets[(int)mBufferType], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
			mPrevValue = mValue;
			mHasUpdatedGPU = true;
			return true;
		}
		return false;
	}

	BaseStructType mValue;

private:
	BaseStructType mPrevValue;
	bool mHasUpdatedGPU{ false };
	int mBindingIndex;
	BufferTypes::Type mBufferType;
	BUF mBuffer;

	template <IsClassOrStruct StructType>
	void addStructToData(const StructType& structToAdd, std::vector<float>& data, int& baseOffset, bool itemAfter) {
		int i{ 0 };
		constexpr int maxI{ boost::pfr::tuple_size<StructType>::value - 1 };

		boost::pfr::for_each_field(structToAdd, [&]<typename MemberType>(const MemberType& value) mutable {
			int baseAlignment{ getBaseAlignment<MemberType>(mBufferType) };

			// Move to start of object (aligned offset)
			if (baseOffset % baseAlignment != 0)
				baseOffset += baseAlignment - (baseOffset % baseAlignment);
			data.resize(numFloats(baseOffset));

			if constexpr (is_array_like_v<MemberType>) {
				addArrayToData<typename MemberType::value_type>(value, data, baseOffset);
			}
			else if constexpr (std::is_class_v<MemberType> && !is_glm_v<MemberType>) {
				addStructToData(value, data, baseOffset, i != maxI);
			}
			else {
				data.insert(data.end(), (float*)&value, (float*)&value + getNumOfFloatsInObject<MemberType>()); // Insert tightly packed floats

				baseOffset += getNumOfFloatsInObject<MemberType>() * sizeof(float);
				data.resize(numFloats(baseOffset)); // Ensure data has possible padding of object (vec3)
			}
			++i;
		});

		// Pad end of struct to 16
		if (baseOffset % 16 != 0 && itemAfter)
			baseOffset += 16 - (baseOffset % 16);
		data.resize(numFloats(baseOffset));
	}

	template <typename ArrayMemberType>
	void addArrayToData(std::span<const ArrayMemberType> arr, std::vector<float>& data, int& baseOffset) {
		// Either contains structs or simple items

		// Array of structs
		if constexpr (std::is_class_v<ArrayMemberType> && !is_glm_v<ArrayMemberType>) {
			for (const ArrayMemberType& item : arr) {
				addStructToData(item, data, baseOffset);
			}
		}
		else {
			int stride{ getBaseAlignment<ArrayMemberType>(mBufferType) };
			for (const ArrayMemberType& item : arr) {
				data.insert(data.end(), (float*)&item, (float*)&item + getNumOfFloatsInObject<ArrayMemberType>()); // Insert tightly packed floats
				baseOffset += stride;
				data.resize(numFloats(baseOffset));
			}
		}
	}
};


#endif