#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "base_window.hpp"
#include "lisk_editor.hpp"

#include "main.h"

struct main_window : public base_window<main_window>
{
	static void help_text()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0, 10.0));
		ImGui::Text(
		  "To open a game either drag and drop it into this window or "
		  "go to [File]->[Open]\n");
		ImGui::PushStyleColor(ImGuiCol_Text, 0xFF8080FF);
		ImGui::Text(
		  "If Source Explorer cannot open/throws errors while opening a file,\n"
		  "please make sure you are running the latest version of Source Explorer,\n"
		  "then SAVE THE ERROR LOG ([File]->[Save Error Log]) and share\n"
		  "it at https://github.com/LAK132/SourceExplorer/issues\n");
		ImGui::PopStyleColor();
		ImGui::Text(
		  "If Source Explorer crashes before you can save the log file,\n"
		  "it will attempt to save it to:\n'%s'\n",
		  lak::debugger.crash_path.string().c_str());
		ImGui::PopStyleVar();
	}

	static void menu_bar(float frame_time)
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Checkbox("Auto-dump Mode", &SrcExp.baby_mode);
			SrcExp.exe.attempt |= ImGui::MenuItem(
			  SrcExp.baby_mode ? "Open And Dump..." : "Open...", nullptr);
			SrcExp.sorted_images.attempt |= ImGui::MenuItem(
			  "Dump Sorted Images...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.images.attempt |=
			  ImGui::MenuItem("Dump Images...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.sounds.attempt |=
			  ImGui::MenuItem("Dump Sounds...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.music.attempt |=
			  ImGui::MenuItem("Dump Music...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.shaders.attempt |=
			  ImGui::MenuItem("Dump Shaders...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.binary_files.attempt |= ImGui::MenuItem(
			  "Dump Binary Files...", nullptr, false, !SrcExp.baby_mode);
			SrcExp.appicon.attempt |=
			  ImGui::MenuItem("Dump App Icon...", nullptr, false, !SrcExp.baby_mode);
			ImGui::Separator();
			SrcExp.error_log.attempt |= ImGui::MenuItem("Save Error Log...");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			ImGui::Text(APP_NAME " by LAK132");
			switch (SrcExp.graphics_mode)
			{
				case lak::graphics_mode::OpenGL:
					ImGui::Text("Using OpenGL %d.%d", opengl_major, opengl_minor);
					break;

				case lak::graphics_mode::Software:
					ImGui::Text("Using Softraster");
					break;

				default: break;
			}
			ImGui::Text("Frame rate %f", std::round(1.0f / frame_time));
			base_window::credits();
			base_window::mode_select();
			if (ImGui::Button("Crash")) FATAL("Force Crashed");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			help_text();
			ImGui::EndMenu();
		}

		ImGui::Checkbox("Color transparency?", &SrcExp.dump_color_transparent);
		ImGui::Checkbox("Force compat mode?", &se::force_compat);
		ImGui::Checkbox("Debug console? (May make SE slow)",
		                &lak::debugger.live_output_enabled);

		if (lak::debugger.live_output_enabled)
		{
			ImGui::Checkbox("Only errors?", &lak::debugger.live_errors_only);
			ImGui::Checkbox("Developer mode?", &lak::debugger.line_info_enabled);
		}
	}

	static void left_region()
	{
		if (SrcExp.loaded)
		{
			if (SrcExp.state.game.title)
				ImGui::Text(
				  "Title: %s",
				  lak::strconv<char>(SrcExp.state.game.title->value).c_str());
			if (SrcExp.state.game.author)
				ImGui::Text(
				  "Author: %s",
				  lak::strconv<char>(SrcExp.state.game.author->value).c_str());
			if (SrcExp.state.game.copyright)
				ImGui::Text(
				  "Copyright: %s",
				  lak::strconv<char>(SrcExp.state.game.copyright->value).c_str());
			if (SrcExp.state.game.output_path)
				ImGui::Text(
				  "Output: %s",
				  lak::strconv<char>(SrcExp.state.game.output_path->value).c_str());
			if (SrcExp.state.game.project_path)
				ImGui::Text(
				  "Project: %s",
				  lak::strconv<char>(SrcExp.state.game.project_path->value).c_str());

			ImGui::Separator();

			if (SrcExp.state.recompiled)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, 0xFF8080FF);
				ImGui::Text(
				  "WARNING: THIS GAME APPEARS TO HAVE BEEN RECOMPILED WITH\n"
				  "AN EXTERNAL TOOL, DUMPING WILL FAIL!\nTHIS IS NOT A BUG.");
				ImGui::PopStyleColor();
			}

			ImGui::Text("New Game: %s", SrcExp.state.old_game ? "No" : "Yes");
			ImGui::Text("Unicode Game: %s", SrcExp.state.unicode ? "Yes" : "No");
			ImGui::Text("Compat Game: %s", SrcExp.state.compat ? "Yes" : "No");
			ImGui::Text("CCN Game: %s", SrcExp.state.ccn ? "Yes" : "No");
			ImGui::Text("2.5+ Game: %s",
			            SrcExp.state.two_five_plus_game ? "Yes" : "No");
			ImGui::Text("Product Build: %zu", (size_t)SrcExp.state.product_build);
			ImGui::Text("Product Version: %zu",
			            (size_t)SrcExp.state.product_version);
			ImGui::Text("Runtime Version: %zu",
			            (size_t)SrcExp.state.runtime_version);
			ImGui::Text("Runtime Sub-Version: %zu",
			            (size_t)SrcExp.state.runtime_sub_version);

			ImGui::Separator();

			SrcExp.state.game.view(SrcExp).UNWRAP();
		}
	}

	static void right_region()
	{
		if (SrcExp.loaded)
		{
			enum mode
			{
				MEMORY,
				IMAGE,
				AUDIO,
				LISK
			};
			static int selected = 0;
			ImGui::RadioButton("Memory", &selected, MEMORY);
			ImGui::SameLine();
			ImGui::RadioButton("Image", &selected, IMAGE);
			ImGui::SameLine();
			ImGui::RadioButton("Audio", &selected, AUDIO);
			ImGui::SameLine();
			ImGui::RadioButton("Lisk", &selected, LISK);

			static bool crypto = false;
			ImGui::Checkbox("Crypto", &crypto);
			ImGui::Separator();

			static bool mem_update   = false;
			static bool image_update = false;
			static bool audio_update = false;
			if (crypto)
			{
				if (base_window::crypto())
					mem_update = image_update = audio_update = true;
				ImGui::Separator();
			}

			switch (selected)
			{
				case MEMORY: base_window::memory_explorer(mem_update); break;
				case IMAGE: base_window::image_explorer(image_update); break;
				case AUDIO: base_window::audio_explorer(audio_update); break;
				case LISK: lisk_editor::draw(); break;
				default: selected = 0; break;
			}
		}
	}

	static void main_region()
	{
		if (!SrcExp.baby_mode && SrcExp.loaded)
		{
			base_window::main_region();
		}
		else
		{
			help_text();
		}

		if (SrcExp.exe.attempt)
			se::AttemptExe(SrcExp);
		else if (SrcExp.images.attempt)
			se::AttemptImages(SrcExp);
		else if (SrcExp.sorted_images.attempt)
			se::AttemptSortedImages(SrcExp);
		else if (SrcExp.appicon.attempt)
			se::AttemptAppIcon(SrcExp);
		else if (SrcExp.sounds.attempt)
			se::AttemptSounds(SrcExp);
		else if (SrcExp.music.attempt)
			se::AttemptMusic(SrcExp);
		else if (SrcExp.shaders.attempt)
			se::AttemptShaders(SrcExp);
		else if (SrcExp.binary_files.attempt)
			se::AttemptBinaryFiles(SrcExp);
		else if (SrcExp.error_log.attempt)
			se::AttemptErrorLog(SrcExp);
		else if (SrcExp.binary_block.attempt)
			se::AttemptBinaryBlock(SrcExp);
	}
};

#endif
