#pragma once
#include "extern/imgui.h"
#include <string>

namespace ImGui {
	namespace Ext {
		extern bool range_scalar(
			const char* label,
			ImGuiDataType_ type,
			void* v_current_min,
			void* v_current_max,
			float v_speed,
			void* v_min,
			void* v_max,
			const char* format,
			const char* format_max
		) noexcept;
	};

	extern void PlotHistogram(
		const char* label,
		const float* values,
		int values_count,
		std::string(*tooltip_format)(int idx0, int idx1, float v0, float v1),
		int values_offset = 0,
		const char* overlay_text = nullptr,
		float scale_min = FLT_MAX,
		float scale_max = FLT_MAX,
		ImVec2 graph_size = { 0, 0 },
		int stride = sizeof(float)
	) noexcept;
};


