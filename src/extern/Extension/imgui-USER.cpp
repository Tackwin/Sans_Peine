#include "imgui-USER.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "extern/imgui_internal.h"

#include <limits>

template<typename T>
bool range_scalar_t(
	ImGuiDataType_ type,
	T* v_current_min,
	T* v_current_max,
	float v_speed,
	T* v_min,
	T* v_max,
	const char* format,
	const char* format_max
) noexcept {
	T min = std::numeric_limits<T>::min();
	T max = std::numeric_limits<T>::max();

	ImGuiContext& g = *GImGui;

	bool value_changed = ImGui::DragScalar(
		"##min",
		type,
		v_current_min,
		v_speed,
		(*v_min >= *v_max) ? &min : v_min,
		(*v_min >= *v_max) ? v_current_max : (*v_max < *v_current_max ? v_max : v_current_max),
		format
	);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
	value_changed |= ImGui::DragScalar(
		"##max",
		type,
		v_current_max,
		v_speed,
		(*v_min >= *v_max) ? v_current_min : (*v_min > *v_current_min ? v_min : v_current_min),
		(*v_min >= *v_max) ? &max : v_max,
		format_max ? format_max : format
	);
	ImGui::PopItemWidth();
	ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

	return value_changed;
}

bool ImGui::Ext::range_scalar(
	const char* label,
	ImGuiDataType_ type,
	void* v_current_min,
	void* v_current_max,
	float v_speed,
	void* v_min,
	void* v_max,
	const char* format,
	const char* format_max
) noexcept {
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
	return false;

	PushID(label);
	BeginGroup();
	PushMultiItemsWidths(2);

	bool value_changed{ false };
	switch (type) {
			
		case ImGuiDataType_S32:{
			value_changed = range_scalar_t<signed int>(
				type,
				(signed int*)v_current_min,
				(signed int*)v_current_max,
				v_speed,
				(signed int*)v_min,
				(signed int*)v_max,
				format,
				format_max
			);
			break;
		}
		case ImGuiDataType_U32: {
			value_changed = range_scalar_t<unsigned int>(
				type,
				(unsigned int*)v_current_min,
				(unsigned int*)v_current_max,
				v_speed,
				(unsigned int*)v_min,
				(unsigned int*)v_max,
				format,
				format_max
			);
			break;
		}
		case ImGuiDataType_S64: {
			value_changed = range_scalar_t<signed long long int>(
				type,
				(signed long long int*)v_current_min,
				(signed long long int*)v_current_max,
				v_speed,
				(signed long long int*)v_min,
				(signed long long int*)v_max,
				format,
				format_max
			);
			break;
		}
		case ImGuiDataType_U64: {
			value_changed = range_scalar_t<unsigned long long int>(
				type,
				(unsigned long long int*)v_current_min,
				(unsigned long long int*)v_current_max,
				v_speed,
				(unsigned long long int*)v_min,
				(unsigned long long int*)v_max,
				format,
				format_max
			);
			break;
		}
		case ImGuiDataType_Float: {
			value_changed = range_scalar_t<float>(
				type,
				(float*)v_current_min,
				(float*)v_current_max,
				v_speed,
				(float*)v_min,
				(float*)v_max,
				format,
				format_max
			);
			break;
		}
		case ImGuiDataType_Double: {
			value_changed = range_scalar_t<double>(
				type,
				(double*)v_current_min,
				(double*)v_current_max,
				v_speed,
				(double*)v_min,
				(double*)v_max,
				format,
				format_max
			);
			break;
		}
	}

	TextUnformatted(label, FindRenderedTextEnd(label));
	EndGroup();
	PopID();

	return value_changed;

}

void PlotEx(
	ImGuiPlotType plot_type,
	const char* label,
	float (*values_getter)(void* data, int idx),
	void* data,
	int values_count,
	std::string(*tooltip_format)(int idx0, int idx1, float v0, float v1),
	int values_offset,
	const char* overlay_text,
	float scale_min,
	float scale_max,
	ImVec2 frame_size
) noexcept {
	using namespace ImGui;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	if (frame_size.x == 0.0f)
		frame_size.x = CalcItemWidth();
	if (frame_size.y == 0.0f)
		frame_size.y = label_size.y + (style.FramePadding.y * 2);

	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
	const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, 0, &frame_bb))
		return;
	const bool hovered = ItemHoverable(frame_bb, id);

	// Determine scale from values if not specified
	if (scale_min == FLT_MAX || scale_max == FLT_MAX)
	{
		float v_min = FLT_MAX;
		float v_max = -FLT_MAX;
		for (int i = 0; i < values_count; i++)
		{
			const float v = values_getter(data, i);
			v_min = ImMin(v_min, v);
			v_max = ImMax(v_max, v);
		}
		if (scale_min == FLT_MAX)
			scale_min = v_min;
		if (scale_max == FLT_MAX)
			scale_max = v_max;
	}

	RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

	if (values_count > 0)
	{
		int res_w = ImMin((int)frame_size.x, values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
		int item_count = values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

		// Tooltip on hover
		int v_hovered = -1;
		if (hovered && inner_bb.Contains(g.IO.MousePos))
		{
			const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
			const int v_idx = (int)(t * item_count);
			IM_ASSERT(v_idx >= 0 && v_idx < values_count);

			const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
			const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
			auto tooltip = tooltip_format(v_idx, v_idx + 1, v0, v1);
			SetTooltip(tooltip.c_str());
			v_hovered = v_idx;
		}

		const float t_step = 1.0f / (float)res_w;
		const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

		float v0 = values_getter(data, (0 + values_offset) % values_count);
		float t0 = 0.0f;
		ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
		float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

		const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
		const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

		for (int n = 0; n < res_w; n++)
		{
			const float t1 = t0 + t_step;
			const int v1_idx = (int)(t0 * item_count + 0.5f);
			IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
			const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
			const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

			// NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
			ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
			ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
			if (plot_type == ImGuiPlotType_Lines)
			{
				window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
			}
			else if (plot_type == ImGuiPlotType_Histogram)
			{
				if (pos1.x >= pos0.x + 2.0f)
					pos1.x -= 1.0f;
				window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
			}

			t0 = t1;
			tp0 = tp1;
		}
	}

	// Text overlay
	if (overlay_text)
		RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

	if (label_size.x > 0.0f)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}


void ImGui::PlotHistogram(
	const char* label,
	const float* values,
	int values_count,
	std::string (*tooltip_format)(int idx0, int idx1, float v0, float v1),
	int values_offset,
	const char* overlay_text,
	float scale_min,
	float scale_max,
	ImVec2 graph_size,
	int stride
) noexcept {
	using namespace ImGui;

	struct Data_Struct
	{
		const float* Values;
		int Stride;

		Data_Struct(const float* values, int stride) { Values = values; Stride = stride; }
	};
	Data_Struct data(values, stride);

	auto array_getter = [](void* data, int idx) {
		Data_Struct* plot_data = (Data_Struct*)data;
		const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
		return v;
	};

	PlotEx(ImGuiPlotType_Histogram, label, array_getter, (void*)& data, values_count, tooltip_format, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

