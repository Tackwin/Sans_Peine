#include "Magnetometer.hpp"

#include "Common.hpp"
#include "extern/imgui.h"

#include "OS/COM.hpp"

constexpr uint8_t Sync_Seq[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void Magnetometer_Window::update() noexcept {
	ImGui::Begin(magnetometer->port.c_str());
	defer{ ImGui::End(); };

	int temp_int = readings_ro_read;
	ImGui::InputInt("Readings", &temp_int, 1);
	readings_ro_read = (size_t)std::max(temp_int, 0);
	ImGui::SameLine();
	if (ImGui::Button("Read")) {
		magnetometer->read(readings_ro_read);
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear")) {
		magnetometer->readings.clear();
	}

	ImGui::Separator();
	
	ImGui::PlotLines("", [](void* data, int idx) {
		Magnetometer* m = (Magnetometer*)data;
		return (float)m->readings[idx].x;
	}, magnetometer, magnetometer->readings.size(), 0, "Reading X", FLT_MAX, FLT_MAX, {0, 200});
	ImGui::PlotLines("", [](void* data, int idx) {
		Magnetometer* m = (Magnetometer*)data;
		return (float)m->readings[idx].y;
	}, magnetometer, magnetometer->readings.size(), 0, "Reading Y", FLT_MAX, FLT_MAX, {0, 200});
	ImGui::PlotLines("", [](void* data, int idx) {
		Magnetometer* m = (Magnetometer*)data;
		return (float)m->readings[idx].z;
	}, magnetometer, magnetometer->readings.size(), 0, "Reading Z", FLT_MAX, FLT_MAX, {0, 200});

	for (auto& r : magnetometer->readings) {
		ImGui::Text("x: %15.d, y: %15.d, z: %15.d", r.x, r.y, r.z);
	}
}

void Magnetometer::read(size_t n) noexcept {
	auto data = read_serial_port(port, n * sizeof(Packet));

	size_t offset{ 0 };
	for (size_t i = 0; i < data.size(); ++i) {
		char seq[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

		bool flag = true;
		for (size_t j = i; j < data.size() && j - i < 10; ++j) {
			if (data[j] != seq[j - i]) {
				flag = false;
				break;
			}
		}

		if (flag) {
			offset = i;
			break;
		}
	}

	size_t n_packet = (data.size() - offset) / sizeof(Packet);
	Packet* packets = (Packet*)(data.data() + offset);

	for (size_t i = 0; i < n_packet; ++i) {
		readings.insert(std::end(readings), BEG_END(packets[i].readings));
	}
}

