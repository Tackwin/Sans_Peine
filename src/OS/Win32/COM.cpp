#include "OS/COM.hpp"
#include "Common.hpp"
#include <Windows.h>

std::vector<uint8_t> read_serial_port(std::string_view port, size_t n) noexcept {
	HANDLE handle;
	auto name = port.data();
	BOOL status;
	DWORD event_mask;

	handle = CreateFileA(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (!handle) return {};
	defer{ CloseHandle(handle); };

	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	status = GetCommState(handle, &dcb);

	if (!status) return {};

	dcb.BaudRate = 115200;
	dcb.ByteSize = 8;
	dcb.StopBits = 1;
	dcb.Parity = NOPARITY;

	status = SetCommState(handle, &dcb);

	if (!status) return {};

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(handle, &timeouts)) return {};

	status = SetCommMask(handle, EV_RXCHAR);

	if (!status) return {};

	status = WaitCommEvent(handle, &event_mask, NULL);

	if (!status) return {};

	std::vector<uint8_t> bytes;
	bytes.resize(n);

	size_t all_byte_read{ 0 };
	do {
		auto Bytes_To_Read_By_Pass = n;
		DWORD bytes_read;

		status = ReadFile(
			handle,
			bytes.data() + all_byte_read,
			std::min(Bytes_To_Read_By_Pass, n - all_byte_read),
			&bytes_read,
			NULL
		);

		all_byte_read += bytes_read;

		if (bytes_read == 0) {
			status = WaitCommEvent(handle, &event_mask, NULL);
			if (!status) return {};
		}
	} while (all_byte_read < n);

	bytes.resize(all_byte_read);

	return bytes;
}

size_t write_serial_port(std::string_view port, std::vector<uint8_t> data) noexcept {
	HANDLE handle;
	auto name = port.data();
	BOOL status;

	handle = CreateFileA(name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (!handle) return 0;
	defer{ CloseHandle(handle); };

	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	status = GetCommState(handle, &dcb);

	if (!status) return 0;

	dcb.BaudRate = 115200;
	dcb.ByteSize = 8;
	dcb.StopBits = 1;
	dcb.Parity = NOPARITY;

	status = SetCommState(handle, &dcb);

	if (!status) return 0;

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(handle, &timeouts)) return false;

	status = SetCommMask(handle, EV_RXCHAR);
	if (!status) return 0;

	DWORD bytes_wrote;

	status = WriteFile(handle, data.data(), data.size(), &bytes_wrote, NULL);

	return bytes_wrote;
}

