#include "OS/COM.hpp"
#include "Common.hpp"
#include <Windows.h>
#include <thread>
#include <chrono>

struct Serial_Port_Window {
	HANDLE handle{ INVALID_HANDLE_VALUE };
	bool connected{ false };
	COMSTAT status;
	DWORD errors;
	size_t baud_rate;
};

Serial_Port::Serial_Port(std::string name) noexcept : name(std::move(name)) {
	plateforme = new Serial_Port_Window;
	auto serial_window = (Serial_Port_Window*)plateforme;

	serial_window->connected = false;

	serial_window->handle = CreateFileA(
		this->name.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (serial_window->handle == INVALID_HANDLE_VALUE) {
		// >TODO: log
		// printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		return;
	}
	DCB dcb = { 0 };

	if (!GetCommState(serial_window->handle, &dcb)) {
		// >TODO: log
		// printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		return;
	}

	serial_window->baud_rate = CBR_115200;
	dcb.BaudRate = serial_window->baud_rate;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(serial_window->handle, &dcb)) {
		// >TODO: log
		// printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		return;
	}
	
	serial_window->connected = true;

	PurgeComm(serial_window->handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

Serial_Port::~Serial_Port() noexcept {
	auto serial_window = (Serial_Port_Window*)plateforme;

	if (serial_window->connected) {
		serial_window->connected = false;
		CloseHandle(serial_window->handle);
	}
	delete serial_window;
}

std::vector<uint8_t> Serial_Port::read(size_t n) noexcept {
	auto serial_window = (Serial_Port_Window*)plateforme;
	DWORD bytes_read{ 0 };

	ClearCommError(serial_window->handle, &serial_window->errors, &serial_window->status);

	auto to_read = std::min((size_t)serial_window->status.cbInQue, n);

	std::vector<uint8_t> result(n);
	ReadFile(serial_window->handle, result.data(), to_read, &bytes_read, NULL);
	result.resize(bytes_read);

	return result;
}

std::vector<uint8_t> Serial_Port::wait_read(size_t n) noexcept {
	auto serial_window = (Serial_Port_Window*)plateforme;
	size_t bytes_read{ 0 };

	std::vector<uint8_t> result(n);
	while (bytes_read < n) {
		ClearCommError(serial_window->handle, &serial_window->errors, &serial_window->status);

		DWORD byte_read_this_time{ 0 };

		auto to_read = std::min((size_t)serial_window->status.cbInQue, n - bytes_read);
		ReadFile(
			serial_window->handle,
			result.data() + bytes_read,
			to_read,
			&byte_read_this_time,
			nullptr
		);
		bytes_read += byte_read_this_time;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);
	}
	result.resize(bytes_read);
	return result;
}

size_t Serial_Port::write(const std::vector<uint8_t>& data) noexcept {
	auto serial_window = (Serial_Port_Window*)plateforme;
	DWORD send{ 0 };

	if (!WriteFile(serial_window->handle, (void*)data.data(), data.size(), &send, 0))
		ClearCommError(serial_window->handle, &serial_window->errors, &serial_window->status);

	return send;
}
