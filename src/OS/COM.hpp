#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct Serial_Port {
	std::string name;

	Serial_Port(std::string name) noexcept;
	~Serial_Port() noexcept;

	Serial_Port(Serial_Port&) = delete;
	Serial_Port& operator=(Serial_Port&) = delete;
	Serial_Port(Serial_Port&&) = default;
	Serial_Port& operator=(Serial_Port&&) = default;

	std::vector<uint8_t> read(size_t n) noexcept;
	std::vector<uint8_t> wait_read(size_t n) noexcept;
	size_t write(const std::vector<uint8_t>& data) noexcept;

private:
	void* plateforme{ nullptr };
};


