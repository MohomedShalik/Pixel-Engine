#pragma once
#include "core/Resources.h"


class ExplorerPopup
{
private:
	bool m_p_open = true;
	bool m_is_path_selected = false;
	std::string m_selected_path = "";
	py::object m_py_explorer;
	ExplorerPopup( const std::string& path) {
		py::module explorer_py = py::module::import("explorer");
		m_py_explorer = explorer_py.attr("Explorer")(path);
	}
	static ExplorerPopup* s_instance;
	int m_selected = -1;
	int m_parent_window = 0; // 0 - start window, 1- object creator, 2- script creator, 3 - object editor
public:

	int getParentWindow() {
		return m_parent_window;
	}
	void setParentWindow(int parent_window) {
		m_parent_window = parent_window;
	}


	static ExplorerPopup* getInstance() {
		if (s_instance == nullptr)  s_instance = new ExplorerPopup("C:/");
		return s_instance;
	}

	void setPathSelectedFalse() {
		m_is_path_selected = false;
	}
	bool isPathSelected() {
		return m_is_path_selected;
	}
	const std::string& getSelectedPath() {
		return m_selected_path;
	}

	std::string getPath() {
		return m_py_explorer.attr("getPath")().cast<std::string>();
	}
	void setPath(const std::string& path) {
		m_py_explorer.attr("setPath")( path );
		m_py_explorer.attr("reload")();
	}

	void render() {
		if (!ImGui::IsPopupOpen("Explorer")) m_p_open = true;
		if (ImGui::BeginPopupModal("Explorer", &m_p_open)) {

			if (!m_p_open) ImGui::CloseCurrentPopup();

			ImGui::SetWindowSize(ImVec2(800, 400), ImGuiCond_Once);

			ImGui::BeginGroup();
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

			// path up button
			if (ImGui::ImageButton(Resources::getOtherIcon("explorer_up"))) {
				m_py_explorer.attr("pathUp")();
			}
			// refresh button
			ImGui::SameLine();
			if (ImGui::ImageButton(Resources::getOtherIcon("explorer_refresh"))) {
				m_py_explorer.attr("reload")();
			}
			// drive select
			ImGui::SameLine();
			const char* drives[] = { "C:/","D:/","E:/" };
			static int current_drive = 0; // C:/
			if (ImGui::Combo("drive", &current_drive, drives, (int)(sizeof(drives) / sizeof(const char*)))) {
				m_py_explorer.attr("setPath")(std::string(drives[current_drive]));
				m_py_explorer.attr("reload")();
			}


			// path text
			std::string path = m_py_explorer.attr("getPath")().cast<std::string>();
			ImGui::Text(path.c_str());

			ImGui::BeginChild("Child2", ImVec2(ImGui::GetWindowSize().x - 15, ImGui::GetWindowSize().y - ImGui::GetCursorPosY() - 50), true);


			for (int i = 0; i < m_py_explorer.attr("getItemCount")().cast<int>(); i++)
			{
				// icon
				std::string item_name = m_py_explorer.attr("getItemName")(i).cast<std::string>();
				if (m_py_explorer.attr("isItemDir")(i).cast<bool>()) {
					ImGui::Image(Resources::getFileFormatIcon("dir_close")); ImGui::SameLine();
				}
				else {
					Resources::drawFileIcon(item_name); ImGui::SameLine();
				}

				// item
				if (ImGui::Selectable(item_name.c_str(), m_selected == i, ImGuiSelectableFlags_AllowDoubleClick)) {
					m_selected = i;
					if (ImGui::IsMouseDoubleClicked(0)) {
						if (m_py_explorer.attr("isItemDir")(i).cast<bool>()) {
							m_selected = -1;
							m_py_explorer.attr("pathIn")(i);
						}
						else {
							m_is_path_selected = true;
							m_selected_path = m_py_explorer.attr("getItemPath")(i).cast<std::string>();
							m_selected = -1;
							ImGui::CloseCurrentPopup();
						}
					}
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();

			// button for dir select and file select
			if (ImGui::Button("Select this folder")) {
				m_is_path_selected = true;

				if (m_selected < 0) {
					m_selected_path = m_py_explorer.attr("getPath")().cast<std::string>();
					m_selected = -1;
					ImGui::CloseCurrentPopup();
				}
				else {  // item selected
					if (!m_py_explorer.attr("isItemDir")(m_selected).cast<bool>()) {
						m_selected_path = m_py_explorer.attr("getPath")().cast<std::string>();
						m_selected = -1;
						m_is_path_selected = true;
						ImGui::CloseCurrentPopup();
					}
					else {
						m_is_path_selected = true;
						m_selected_path = m_py_explorer.attr("getItemPath")(m_selected).cast<std::string>();
						m_selected = -1;
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cencel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndGroup();
			ImGui::EndPopup();
		}
	}

private:
	
};