#pragma once
#include "grad.h"
#include "city.h"
#include "rockets.h"
#include "randomCoordinates.h"
#include "getheight.h"

class GUI {
private:
	std::string control = "\nW - Raise launcher\nS - Lower launcher\nA - Launcher to left\n"
		"D - launcher to right\nSPACE - Volley\n< - Turn cam to left\n> - Turn cam to right\n"
		"^ - Turn cam to up\nv - Turn cam to down\nNPAD 7 - Camera to forward\nNPAD 9 - Camera to back"
		"\nNPAD 6 - Camera to right\nNPAD 4 - Camera to left\nNPAD 8 - Camera to up\nNPAD 2 - Camera to down";

public:
	void render(float fps, GradLauncher& g, City& city, int totalB, int& score, float& dmg,
		float& sumDestructions, int rCount) {
		Vec3 hit = computeImpact({ 0.0f, getHeight(0.0f, 0.0f), 0.0f }, speed, g.pitch, g.yaw);

		for (auto& b : city.buildings) sumDestructions += b.destructionLevel;
		
		sumDestructions /= rCount;

		ImGui::Begin("Seetings & Info");

		if (ImGui::Button("Reset city")) {
			auto coords = randomCoordinates();
			city.config({ coords[0],getHeight(coords[0],coords[1]),coords[1]},totalB,rCount);
		}
		
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Score: %d", score);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Volley efficiency: %.2f",sumDestructions*100);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Launcher yaw: %.1f", g.yaw);
		ImGui::Text("Launcher pitch: %.1f", g.pitch);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SliderFloat("Rocket damage", &dmg, 0.1f, 0.7f);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("City center coords: (%.1f; %.1f; %.1f)", city.center.x, city.center.y, -city.center.z);
		ImGui::Text("Approx. Impact: (%.1f; %.1f; %.1f)", -hit.x,hit.y,hit.z);
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("Control: %s", control.c_str());
		ImGui::End();
	}
};