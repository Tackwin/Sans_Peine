#pragma once
#include <string>
#include <vector>
#include <cstdint>

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

	std::string port;

	std::vector<Reading> readings;

	void read(size_t n) noexcept;
};

struct Magnetometer_Window {
	Magnetometer* magnetometer{ nullptr };

	bool open{ false };

	size_t readings_ro_read{ 0 };

	void update() noexcept;
};
