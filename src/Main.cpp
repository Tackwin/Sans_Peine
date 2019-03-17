#include <SFML/Graphics.hpp>
#include "extern/imgui.h"
#include "extern/imgui-SFML.h"

#include "Magnetometer.hpp"

int main(int, char**) noexcept {

	sf::RenderWindow window(sf::VideoMode{ 1600, 900 }, "Sans Peine");

	ImGui::SFML::Init(window);

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	bool show_demo{ false };
	bool dockspace_open{ true };

	Magnetometer main_mag;
	main_mag.port = "\\\\.\\COM3";
	Magnetometer_Window main_mag_window;
	main_mag_window.magnetometer = &main_mag;

	sf::Clock dt_clock;
	while (window.isOpen()) {
		float dt = dt_clock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::F11) show_demo = !show_demo;
			}
		}
		ImGui::SFML::Update(window, sf::seconds(dt));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
		ImGui::PopStyleVar();
		ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.0f, 0.0f));

		main_mag_window.update();
		if (show_demo) ImGui::ShowDemoWindow(&show_demo);

		ImGui::End();
		ImGui::PopStyleVar(2);

		window.clear({ 33, 33, 33, 255 });

		ImGui::SFML::Render(window);
		window.display();
	}

}
