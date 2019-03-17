#pragma once
#include <vector>
#include <string>
#include <cstdint>

extern std::vector<uint8_t> read_serial_port(std::string_view port, size_t n) noexcept;
extern size_t write_serial_port(std::string_view port, std::vector<uint8_t> data) noexcept;


