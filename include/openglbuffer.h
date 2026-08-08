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
#include <cstring>

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

template<glm::length_t C, glm::length_t R, glm::qualifier Q>
struct is_glm<glm::mat<C, R, float, Q>> : std::true_type {};

template <typename T>
inline constexpr bool is_glm_v = is_glm<T>::value;

// Concepts
template <typename T>
concept IsStructLike = std::is_class_v<T> && !is_glm_v<T> && !is_array_like_v<T>;

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

namespace {
	inline int numFloats(int numBytes) {
		return numBytes / 4;
	}

	template <typename>
	inline constexpr bool always_false_v = false;

	template <typename StructType>
	inline int getMaxBaseAlignmentOfStruct(BufferTypes::Type bufferType);

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
		else if constexpr (IsStructLike<T>)
			return bufferType == BufferTypes::uniform ? 16 : getMaxBaseAlignmentOfStruct<T>(bufferType);
		else if constexpr (IsArrayLike<T>)
			return bufferType == BufferTypes::uniform ? 16 : getBaseAlignment<typename T::value_type>(bufferType);
		else
			static_assert(always_false_v<T>, "getBaseAlignment<T> requires T to be one of int, float, glm::vec2, glm::vec3, glm::vec4, glm::matCxR, std::vector, std::array, any struct");
	}

	// Getting max alignment of struct
	template <typename T>
	inline void processType(int& currentMax, BufferTypes::Type bufferType) {
		int alignment{ getBaseAlignment<T>(bufferType) };
		if (alignment > currentMax) {
			currentMax = alignment;
		}
	}

	template <typename StructType, size_t... Is>
	inline int processIs(std::index_sequence<Is...>, BufferTypes::Type bufferType) {
		int currentMax{};
		(processType<boost::pfr::tuple_element_t<Is, StructType>>(currentMax, bufferType), ...);
		return currentMax;
	}

	template <typename StructType>
	inline int getMaxBaseAlignmentOfStruct(BufferTypes::Type bufferType) {
		constexpr size_t fieldCount{ boost::pfr::tuple_size_v<StructType> };
		return processIs<StructType>(std::make_index_sequence<fieldCount>(), bufferType);
	}
	// Done

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
template <IsStructLike BaseStructType>
class OpenGLBuffer {
public:

	OpenGLBuffer(int bindingIndex, BufferTypes::Type bufferType, const BaseStructType& value)
		: mBindingIndex{ bindingIndex }
		, mBufferType{ bufferType }
		, mValue{ value }
	{
		mBuffer.bind(BufferTypes::bufferTargets[(int)bufferType]);
		glBindBufferBase(BufferTypes::bufferTargets[(int)mBufferType], mBindingIndex, mBuffer);
		updateGPU();
	}

	OpenGLBuffer(int bindingIndex, BufferTypes::Type bufferType)
		: mBindingIndex{ bindingIndex }
		, mBufferType{ bufferType }
	{
		mBuffer.bind(BufferTypes::bufferTargets[(int)bufferType]);
		glBindBufferBase(BufferTypes::bufferTargets[(int)mBufferType], mBindingIndex, mBuffer);
	}

	bool updateGPU(bool force = false) {
		if (!mHasUpdatedGPU || mPrevValue != mValue || force) {
			std::vector<float> data;
			int baseOffset{ 0 };
			addStructToData(mValue, data, baseOffset, -1, false);

			mBuffer.bind(BufferTypes::bufferTargets[(int)mBufferType]);
			glBufferData(BufferTypes::bufferTargets[(int)mBufferType], data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
			mPrevValue = mValue;
			mHasUpdatedGPU = true;
			return true;
		}
		return false;
	}

	BaseStructType readGPU() const {
		// Get GPU byte count
		int gpuFloatCount;
		{
			std::vector<float> data;
			int baseOffset{ 0 };
			addStructToData(mValue, data, baseOffset, getMaxBaseAlignmentOfStruct<BaseStructType>(mBufferType), false);
			gpuFloatCount = data.size();
		}
		std::vector<float> gpuData(gpuFloatCount);
		mBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, gpuFloatCount * sizeof(float), &(gpuData[0]));

		BaseStructType returnValue;
		int baseOffset{};
		readStructFromData(returnValue, gpuData, baseOffset, -1, false);
		return returnValue;
	}

	BaseStructType mValue;

private:
	BaseStructType mPrevValue;
	bool mHasUpdatedGPU{ false };
	int mBindingIndex;
	BufferTypes::Type mBufferType;
	BUF mBuffer;

	template <IsStructLike StructType>
	void addStructToData(const StructType& structToAdd, std::vector<float>& data, int& baseOffset, int structBaseAlignment, bool isSubStructure) const {
		int i{ 0 };
		constexpr int maxI{ boost::pfr::tuple_size<StructType>::value - 1 };

		boost::pfr::for_each_field_with_name(structToAdd, [&]<typename MemberType>(std::string_view name, const MemberType& value) mutable {
			int baseAlignment{ getBaseAlignment<MemberType>(mBufferType) };

			// Move to start of object (aligned offset)
			if (baseOffset % baseAlignment != 0)
				baseOffset += baseAlignment - (baseOffset % baseAlignment);
			data.resize(numFloats(baseOffset));

			if constexpr (is_array_like_v<MemberType>) {
				addArrayToData<MemberType, typename MemberType::value_type>(value, data, baseOffset);
			}
			else if constexpr (IsStructLike<MemberType>) {
				addStructToData(value, data, baseOffset, baseAlignment, true);
			}
			else {
				if constexpr (std::is_same_v<MemberType, bool>) {
					static int zero{ 0 };
					static int one{ 1 };
					data.push_back(*((float*)&(value ? one : zero)));
				}
				else {
					// Insert tightly packed floats
					data.insert(data.end(), (float*)&value, (float*)&value + getNumOfFloatsInObject<MemberType>()); // TODO matrices beyond mat4
				}

				baseOffset += getNumOfFloatsInObject<MemberType>() * sizeof(float);
			}
			++i;
		});

		// Pad end of struct to base alignment
		if (baseOffset % structBaseAlignment != 0 && isSubStructure)
			baseOffset += structBaseAlignment - (baseOffset % structBaseAlignment);
		data.resize(numFloats(baseOffset));
	}

	template <typename ArrayType, typename ArrayMemberType>
	void addArrayToData(std::span<const ArrayMemberType> arr, std::vector<float>& data, int& baseOffset) const {
		// Either contains structs or simple items

		// Array of structs
		if constexpr (std::is_class_v<ArrayMemberType> && !is_glm_v<ArrayMemberType>) {
			for (const ArrayMemberType& item : arr) {
				addStructToData(item, data, baseOffset, getMaxBaseAlignmentOfStruct<ArrayMemberType>(mBufferType), true);
			}
		}
		else {
			int stride;
			if constexpr (std::is_same_v<ArrayMemberType, glm::mat4>) { // TODO cover all matrix types properly (mat4 is the only trivial one)
				stride = 64; // Techincally the stride is 16 but the items are the column vectors, not the entire matrix, but this works for just mat4
			}
			else {
				stride = getBaseAlignment<ArrayType>(mBufferType);
			}
			for (const ArrayMemberType& item : arr) {
				data.insert(data.end(), (float*)&item, (float*)&item + getNumOfFloatsInObject<ArrayMemberType>()); // Insert tightly packed floats
				baseOffset += stride;
				data.resize(numFloats(baseOffset));
			}
		}
	}

	template <IsStructLike StructType>
	void readStructFromData(StructType& structToWrite, const std::vector<float>& data, int& baseOffset, int structBaseAlignment, bool isSubStructure) const {
		int i{ 0 };
		constexpr int maxI{ boost::pfr::tuple_size<StructType>::value - 1 };

		boost::pfr::for_each_field_with_name(structToWrite, [&]<typename MemberType>(std::string_view name, MemberType& value) mutable {
			int baseAlignment{ getBaseAlignment<MemberType>(mBufferType) };

			// Move to start of object (aligned offset)
			if (baseOffset % baseAlignment != 0)
				baseOffset += baseAlignment - (baseOffset % baseAlignment);

			if constexpr (is_array_like_v<MemberType>) {
				readArrayFromData<MemberType, typename MemberType::value_type>(value, data, baseOffset);
			}
			else if constexpr (IsStructLike<MemberType>) {
				readStructFromData(value, data, baseOffset, baseAlignment, true);
			}
			else {
				if constexpr (std::is_same_v<MemberType, bool>) {
					bool gpuBool = (bool)(data[numFloats(baseOffset)]);
					value = gpuBool;
				}
				else {
					// Read tightly packed floats
					std::memcpy(&value, &data[numFloats(baseOffset)], getNumOfFloatsInObject<MemberType>() * sizeof(float)); // TODO matrices beyond mat4
				}

				baseOffset += getNumOfFloatsInObject<MemberType>() * sizeof(float);
			}
			++i;
		});

		// Pad end of struct to base alignment
		if (baseOffset % structBaseAlignment != 0 && isSubStructure)
			baseOffset += structBaseAlignment - (baseOffset % structBaseAlignment);
	}

	template <typename ArrayType, typename ArrayMemberType>
	void readArrayFromData(ArrayType& arr, const std::vector<float>& data, int& baseOffset) const {
		// Either contains structs or simple items

		// Array of structs
		if constexpr (std::is_class_v<ArrayMemberType> && !is_glm_v<ArrayMemberType>) {
			for (const ArrayMemberType& item : arr) {
				readStructFromData(item, data, baseOffset, getMaxBaseAlignmentOfStruct<ArrayMemberType>(mBufferType), true);
			}
		}
		else {
			int stride;
			if constexpr (std::is_same_v<ArrayMemberType, glm::mat4>) { // TODO cover all matrix types properly (mat4 is the only trivial one)
				stride = 64; // Techincally the stride is 16 but the items are the column vectors, not the entire matrix, but this works for just mat4
			}
			else {
				stride = getBaseAlignment<ArrayType>(mBufferType);
			}
			for (size_t i{ 0 }; i < arr.size(); ++i) {

				std::memcpy(&(arr[i]), &(data[numFloats(baseOffset)]), getNumOfFloatsInObject<ArrayMemberType>() * sizeof(float)); // Read tightly packed floats
				baseOffset += stride;
			}
		}
	}
};


#endif