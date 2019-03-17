#pragma once
#include <set>
#include <random>
#include <memory>
#include <string>
#include <bitset>
#include <filesystem>
#include <type_traits>
#include <string_view>
#include <unordered_set>

#include <SFML/Graphics.hpp>

// just for convenience
#include "Utils/Defer.hpp"

using i08 = std::int8_t;
using u08 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

#define BEG_END(x) std::begin((x)), std::end((x))

namespace xstd {
	template<typename T>
	constexpr int sign(T x) noexcept {
		return (T(0) < x) - (x < T(0));
	}

	template<typename T, typename U>
	std::enable_if_t<
		std::is_convertible_v<T, std::string> && std::is_constructible_v<U, std::string>,
		std::string
	> append(T a, U b) {
		return std::string{ a } +b;
	}

	template<typename T>
	constexpr inline std::size_t hash_combine(std::size_t seed, const T& v) noexcept {
		std::hash<T> h;
		seed ^= h(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
	constexpr inline std::size_t hash_combine(std::size_t a, size_t b) noexcept {
#pragma warning(push)
#pragma warning(disable: 4307)
		return a + 0x9e3779b9u + (b << 6) + (b >> 2);
#pragma warning(pop)
	}

	template<size_t S>
	constexpr std::bitset<S> full_bitset() noexcept {
		std::bitset<S> bitset;
		for (size_t i = 0; i < S; ++i) bitset.set(i, true);
		return bitset;
	}

	template<size_t S>
	constexpr std::bitset<S> consecutive_to_bitset() noexcept {
		return std::bitset<S>{};
	}

	template<size_t S, typename... Args>
	constexpr std::bitset<S> consecutive_to_bitset(size_t x, Args... args) noexcept {
		std::bitset<S> bs;
		bs.set(x, true);
		if constexpr (sizeof...(Args) == 0) {
			return bs;
		}

		return bs | consecutive_to_bitset<S>(args...);
	}

	constexpr int case_insensitive_compare(std::string_view a, std::string_view b) noexcept {
		for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
			if (std::tolower(a[i]) < std::tolower(b[i])) return -1;
			if (std::tolower(a[i]) > std::tolower(b[i])) return 1;
		}
		if (a.size() != b.size()) {
			return a.size() < b.size() ? -1 : 1;
		}
		return 0;
	}
};

namespace std {
	template<typename T>
	struct hash<std::pair<T, T>> {
		size_t operator()(const std::pair<size_t, size_t>& x) const noexcept {
			return xstd::hash_combine(std::hash<T>()(x.first), x.second);
		}
	};

	template<>
	struct hash<std::set<size_t>> {
		size_t operator()(const std::set<size_t>& x) const noexcept {
			size_t seed = 0;
			for (const auto& v : x) {
				seed = xstd::hash_combine(seed, v);
			}
			return seed;
		}
	};
	template<>
	struct hash<std::unordered_set<size_t>> {
		size_t operator()(const std::unordered_set<size_t>& x) const noexcept {
			size_t seed = 0;
			for (const auto& v : x) {
				seed = xstd::hash_combine(seed, v);
			}
			return seed;
		}
	};
};

namespace C {
	extern const std::filesystem::path Assets_Path;
	extern const std::filesystem::path Exe_Dir;

	constexpr u32 Width = 1280;
	constexpr u32 Height = 800;
	constexpr double Ratio = (double)Width / (double)Height;

	constexpr size_t operator""_id(const char* user, size_t size) {
		size_t seed = 0;
		for (size_t i = 0; i < size; ++i) seed = xstd::hash_combine(seed, (size_t)user[i]);
		return seed;
	}
};
using namespace C;
