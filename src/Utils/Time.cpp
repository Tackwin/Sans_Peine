#include "Time.hpp"
#include <ctime>

std::string get_minimum_string_from_timestamp(time_t timestamp) noexcept {
	char buffer[256];

	std::uint64_t milliseconds = timestamp;
	std::uint64_t seconds = milliseconds / 1'000;

	int len = 0;
	if (seconds < 5) {
		len = sprintf_s(
			buffer, "%llus %llums", seconds, milliseconds % 1000
		);
	}
	else if (seconds < 60) {
		len = sprintf_s(buffer, "%llus", seconds);
	}
	else if (seconds < 10 * 60) {
		len = sprintf_s(buffer, "%llum %llus", seconds / 60, seconds % 60);
	}
	else if (seconds < 60 * 60) {
		len = sprintf_s(buffer, "%llum", seconds / 60);
	}
	else if (seconds < 24 * 60 * 60) {
		len = sprintf_s(
			buffer,
			"%lluh %llum",
			seconds / (60 * 60),
			(seconds % (60 * 60)) / 60
		);
	}
	else if (seconds < 24 * 60 * 60 * 2) {
		len = sprintf_s(
			buffer,
			"%llud %lluh",
			seconds / (24 * 60 * 60),
			(seconds % (24 * 60 * 60)) / (60 * 60)
		);
	}
	else {
		len = sprintf_s(buffer, "%llud", seconds / (24 * 60 * 60));
	}
	if (len >= 256) return "";
	buffer[len] = '\0';

	std::string tooltip{ buffer };
	return tooltip;
}

