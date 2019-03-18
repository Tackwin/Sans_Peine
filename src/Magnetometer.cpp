#include "Magnetometer.hpp"

#include "Common.hpp"
#include "extern/imgui.h"

#include "OS/COM.hpp"

#include "Page.hpp"

constexpr uint8_t Sync_Seq[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

Magnetometer::Magnetometer(std::string port) noexcept : serial(port), name(port) {
}

void Magnetometer_Window::update() noexcept {
	ImGui::Begin(magnetometer->name.c_str());
	defer{ ImGui::End(); };

	int temp_int = readings_ro_read;
	ImGui::InputInt("Readings", &temp_int, 1);
	readings_ro_read = (size_t)std::max(temp_int, 0);
	size_t old_size = magnetometer->readings.size();
	magnetometer->read(readings_ro_read);

	for (size_t i = old_size; i < magnetometer->readings.size(); ++i) {
		auto& it = magnetometer->readings[i];

		page->points.push_back({ (float)it.x, (float)it.y });
		// page->points.push_back({
		// 	(std::signbit(it.x) ? -1 : 1) * std::sqrtf((float)std::abs(it.x)),
		// 	(std::signbit(it.y) ? -1 : 1) * std::sqrtf((float)std::abs(it.y))
		// });
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear")) {
		magnetometer->readings.clear();
		magnetometer->raw_points.clear();
		page->points.clear();
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(50);
	ImGui::DragInt("Smooth", &magnetometer->smooth_sampling, 1, 10, 100);
	ImGui::PopItemWidth();


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

	for (size_t i = magnetometer->readings.size() - 1; i + 1 > 0; --i) {
		auto& r = magnetometer->readings[i];
		ImGui::Text("x: %15.2f, y: %15.2f, z: %15.2f", r.x, r.y, r.z);
	}
}

void Magnetometer::read(size_t n) noexcept {
	thread_local std::vector<uint8_t> received_data;

	auto data = serial.read(n * sizeof(Packet));
	received_data.insert(std::end(received_data), BEG_END(data));


	size_t offset{ 0 };
	for (size_t i = 0; i < received_data.size(); ++i) {
		char seq[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

		bool flag = true;
		for (size_t j = i; j < received_data.size() && j - i < 10; ++j) {
			if (received_data[j] != seq[j - i]) {
				flag = false;
				break;
			}
		}

		if (flag) {
			offset = i;
			break;
		}
	}

	size_t n_packet = (received_data.size() - offset) / sizeof(Packet);
	Packet* packets = (Packet*)(received_data.data() + offset);

	size_t Sampling_Smooth = smooth_sampling;
	size_t Sampling_Smooth_2 = Sampling_Smooth / 2;

	for (size_t i = 0; i < n_packet; ++i) {
		for (auto& x : packets[i].readings) {
			raw_points.push_back(Vector3f{(float)x.x, (float)x.y, (float)x.z});
		}
	}

	received_data.erase(
		std::begin(received_data),
		std::begin(received_data) + offset + n_packet * sizeof(Packet)
	);

	for (size_t i = Sampling_Smooth_2 + readings.size(); i + Sampling_Smooth_2 < raw_points.size(); ++i) {
		Vector3f avg{ 0, 0, 0 };
		for (size_t j = 0; j < Sampling_Smooth; ++j) avg += raw_points[i - Sampling_Smooth_2 + j];
		avg /= Sampling_Smooth;

		readings.push_back(avg);
	}
}

