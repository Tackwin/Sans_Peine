#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "OS/COM.hpp"

#include "Vector.hpp"

struct Page;

struct Magnetometer {
#pragma pack(push, 1)
	struct Reading {
		int32_t x;
		int32_t y;
		int32_t z;
	};

	struct Packet {
		uint8_t sync_seq[10];
		Reading readings[10];
	};
#pragma pack(pop)

	Serial_Port serial;
	std::vector<Vector3f> readings;
	std::vector<Vector3f> raw_points;

	std::string name;

	int smooth_sampling{ 10 };

	Magnetometer(std::string port) noexcept;

	void read(size_t n) noexcept;
};

struct Magnetometer_Window {
	Magnetometer* magnetometer{ nullptr };
	Page* page{ nullptr };

	bool open{ false };

	size_t readings_ro_read{ 1 };

	void update() noexcept;
};
