#include "pch.h"
#include "Drawing.h"
#include "MainThread.h"
#include "PlayableCharactersManager.h"
#include "Tools/Colors.hpp"
#include "Files/PaletteFiles.h"
#include "Files/GroupJSONFiles.h"
#include "ImGuiCustom.h"
#include "Files/Config.h"
#include "AutoLoadPalette.h"

auto DrawingLogger = LOGGER::createLocal("Drawing", LogLevel::GENERAL_LOG);

PlayableCharactersManager& charMgr = PlayableCharactersManager::instance();
int& Curent_Index = PlayableCharactersManager::GetCurrentCharacterIndex();
auto& Curent_Char = PlayableCharactersManager::GetCurrentCharacter();

void Drawing::Draw()
{
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		bDrawAll = !bDrawAll;
	}

	if (bDrawAll)
	{
		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);

		ImGui::Begin(lpWindowName, &bDrawAll, WindowFlags);
		{
			
			DrawMenuBar();
			FileDialog();
			if (ImGui::BeginTabBar("##TabBar")) {

				if (ImGui::BeginTabItem("Palette")) {
					DrawPaletteTabItem();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Auto Load Palette")) {
					DrawAutoLoadPaletteTabItem();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Test Page")) {
					ImGui::Text("Did you want find something here?");

					static bool ShowDemoWindow = true;
					if (ShowDemoWindow) {
						ImGui::ShowDemoWindow(&ShowDemoWindow);
					}
					ImGui::Checkbox("Show Demo Window", &ShowDemoWindow);



					//Test things
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			
		}
		ImGui::End();

	}
}



void Drawing::DrawMenuBar() {
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (Curent_Index == -1) { //If we haven't Loaded Character;
				ImGui::TextDisabled("Save Palette");
				if (ImGui::IsItemHovered()) {
					if (ImGui::BeginItemTooltip())
					{
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("You need to load/select a character first!");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				}

				ImGui::TextDisabled("Load Palette");
				if (ImGui::IsItemHovered()) {
					if (ImGui::BeginItemTooltip())
					{
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("You need to load/select a character first!");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}
				}

			}
			else {
				if (ImGui::MenuItem("Save Palette"))
				{
					IGFD::FileDialogConfig config;
					config.path = ".";
					config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
					ImGuiFileDialog::Instance()->OpenDialog("SavePaletteFile", "Save Palette File", ".pal", config);
				}

				if (ImGui::MenuItem("Load Palette"))
				{
					IGFD::FileDialogConfig config;
					config.path = ".";
					ImGuiFileDialog::Instance()->OpenDialog("LoadPaletteFile", "Load Palette File", ".pal", config);

				}


			}
			ImGui::Separator();
			if (ImGui::MenuItem("Load JSON (Characters parts)"))
			{
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("LoadJSON", "Load JSON (Characters parts)", ".json", config);
			}
			ImGui::EndMenu();
		}

		// Кнопка About прямо в меню баре
		if (ImGui::MenuItem("About"))
		{
			// Открываем окно About при нажатии
			bDrawAboutWindow = true;

		}
		ImGui::EndMenuBar();

		DrawAboutWindow();

		ImGui::SetNextWindowSize(vFileDialogSize, ImGuiCond_Once);



	}
}

void Drawing::DrawAboutWindow() {
	if (!bDrawAboutWindow) return;
	ImGui::Begin("About", &bDrawAboutWindow);
	{
		ImGui::Text("Skullgirls Pallete Editor 2nd Enocore (DLL Version)");
		ImGui::TextDisabled("Version: v0.6");
		ImGui::TextDisabled("Author: ImpDi");
		ImGui::Text("Also, check our Discord: ");
		ImGui::TextLinkOpenURL("Discord", "https://discord.gg/4ufGJQjkpc");
		ImGui::Separator();
		if (ImGui::Button("OK"))
		{
			bDrawAboutWindow = false;
		}
	}
	ImGui::End();
}

void Drawing::DrawPaletteTabItem() {

	if (MainThread::Match_Readed != true) {
		ImGui::Text("Start a match to use the editor.");
		return;
	}
	DrawPlayableCharactersComboBox();
	if (charMgr.GetCurrentCharacterIndex() == -1) return;

	ImGui::Spacing();

	DrawCharacterPaletteNumSlider();

	ImGui::Separator();

	DrawCharacterOptions();

	ImGui::Separator();

	DrawCharacterColors();

}


void Drawing::DrawPlayableCharactersComboBox() {
	const auto& allCharsNames = charMgr.GetCharacterNames();

	std::string preview_text = "Select";
	if (Curent_Index != -1) {
		preview_text = allCharsNames[Curent_Index].value();
	}
	ImGui::Text("Select character:");

	if (ImGui::BeginCombo("##CharSelect", preview_text.c_str())) {

		for (int i = 0; i < MAX_PLAYABLE_CHARACTERS; i++) {
			if (!allCharsNames[i].has_value()) continue; //If we haven't Playable Character in Array

			bool is_selected = (Curent_Index == i);

			std::string display_name = allCharsNames[i].value();

			if (i < 3) {
				display_name += " (Player 1)";
			}
			else {
				display_name += " (Player 2)";
			}

			if (ImGui::Selectable(display_name.c_str(), is_selected)) {
				if (Curent_Index == i) {
					LOG_LOCAL_WARN(DrawingLogger, "We are trying to select an already selected character.");
					continue;
				}
				// Сохраняем ID выбранного персонажа
				LOG_LOCAL_DEBUG(DrawingLogger, "Choose new Playable Character ", allCharsNames[i].value(), " at ", i);
				charMgr.SetCurrentCharacterIndex(i);
				charMgr.LoadCharacter();
				LOG_DEBUG("Loading character from slot: ", i);
				LOG_DEBUG("Character name: ", Curent_Char.Char_Name);
				LOG_DEBUG("Character count of palette: ", Curent_Char.Max_Palette_Num);
				LOG_DEBUG("Character current num of Palette: ", Curent_Char.Current_Palette_Num);
				LOG_DEBUG("Character count of colors: ", Curent_Char.Num_Of_Color);

			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}

		}
		ImGui::EndCombo();
	}

}

void Drawing::DrawCharacterOptions() {
	ImGui::Text("Options: ");
	//We need to create "intermediate" bool
	bool Drawing_NODisplayChar = MainThread::b_NODisplayChar.load();
	if (ImGui::Checkbox("Don't display characters", &Drawing_NODisplayChar)) {
		MainThread::b_NODisplayChar.store(Drawing_NODisplayChar); //And now, we save this in our atomic bool
	}
	bool Drawing_NODisplayShadows = MainThread::b_NODisplayShadows.load();
	if (ImGui::Checkbox("Don't display shadows", &Drawing_NODisplayShadows)) {
		MainThread::b_NODisplayShadows.store(Drawing_NODisplayShadows); //And now, we save this in our atomic bool
	}
	bool Drawing_DisplaySuperShadows = MainThread::b_DisplaySuperShadows.load();
	if (ImGui::Checkbox("Display super shadow", &Drawing_DisplaySuperShadows)) {
		MainThread::b_DisplaySuperShadows.store(Drawing_DisplaySuperShadows); //And now, we save this in our atomic bool
	}

	ImGui::Checkbox("Group Character Parts", &bDrawColorGroup);
}

void Drawing::DrawCharacterPaletteNumSlider() {
	int displayValue = Curent_Char.Current_Palette_Num + 1;

	if (ImGui::SliderInt("Palette Number##", &displayValue, 1, Curent_Char.Max_Palette_Num)) {
		// Вычитаем 1 для получения реального индекса (0-based)
		int newPaletteIndex = displayValue - 1;

		// Проверяем, действительно ли значение изменилось
		if (newPaletteIndex != Curent_Char.Current_Palette_Num) {
			PlayableCharactersManager::ChangePaletteNumber(newPaletteIndex);
		}
	}

	// Можно добавить отображение текущего значения
	ImGui::SameLine();
	ImGui::Text("(%d/%d)", displayValue, Curent_Char.Max_Palette_Num);
}

void Drawing::DrawCharacterColors() {

	ImGui::Text("Colors: ");
	ImGui::Spacing();

	//First of all, we change BGRA to RGBA
	ImU32 colorLineColorRGBA_U32 = ColorsTools::SwapRBChannels(Curent_Char.LineColor);
	//Then, we change ImU32 to ImVec4, for ColorEdit4 correct work
	ImVec4 colorLineColorVec = ImGui::ColorConvertU32ToFloat4(colorLineColorRGBA_U32);
	//Then, we do the same for ShadowColor1
	ImU32 colorShadowColor1RGBA_U32 = ColorsTools::SwapRBChannels(Curent_Char.SuperShadowColor1);
	ImVec4 colorShadowColor1Vec = ImGui::ColorConvertU32ToFloat4(colorShadowColor1RGBA_U32);
	//And ShadowColor2
	ImU32 colorShadowColor2RGBA_U32 = ColorsTools::SwapRBChannels(Curent_Char.SuperShadowColor2);
	ImVec4 colorShadowColor2Vec = ImGui::ColorConvertU32ToFloat4(colorShadowColor2RGBA_U32);

	//Be careful - We will ColorEdit3, instead of ColorEdit4, becouse Alpha Channel don't change anything
	if (ImGui::ColorEdit3("Line Color", &colorLineColorVec.x, ImGuiColorEditFlags_NoInputs)) {
		LOG_LOCAL_DEBUG(DrawingLogger, "Change Line color: ");
		LOG_LOCAL_VARIABLE(DrawingLogger, colorLineColorVec.x);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorLineColorVec.y);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorLineColorVec.z);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorLineColorVec.w);
		LOG_LOCAL_DEBUG(DrawingLogger, "Red, Green, Blue, Alpha");
		//First, we change from float ImVec4 to ImU32 (__int32)
		ImU32 ColorToWrite = ImGui::ColorConvertFloat4ToU32(colorLineColorVec);
		//Then, we change BGRA to RGBA
		ColorToWrite = ColorsTools::SwapRBChannels(ColorToWrite);
		PlayableCharactersManager::ChangeOptionPaletteColor(ColorToWrite, ColorOptionFlag::FLAG_LINE_COLOR);
	}

	ImGui::SameLine();

	if (ImGui::ColorEdit3("Shadow Color 1", &colorShadowColor1Vec.x, ImGuiColorEditFlags_NoInputs)) {
		LOG_LOCAL_DEBUG(DrawingLogger, "Change Shadow Color: ");
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor1Vec.x);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor1Vec.y);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor1Vec.z);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor1Vec.w);
		LOG_LOCAL_DEBUG(DrawingLogger, "Red, Green, Blue, Alpha");
		//First, we change from float ImVec4 to ImU32 (__int32)
		ImU32 ColorToWrite = ImGui::ColorConvertFloat4ToU32(colorShadowColor1Vec);
		//Then, we change BGRA to RGBA
		ColorToWrite = ColorsTools::SwapRBChannels(ColorToWrite);
		PlayableCharactersManager::ChangeOptionPaletteColor(ColorToWrite, ColorOptionFlag::FLAG_SUPER_SHADOW_1);
	}

	ImGui::SameLine();

	if (ImGui::ColorEdit3("Shadow Color 2", &colorShadowColor2Vec.x, ImGuiColorEditFlags_NoInputs)) {
		LOG_LOCAL_DEBUG(DrawingLogger, "Change Line color: ");
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor2Vec.x);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor2Vec.y);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor2Vec.z);
		LOG_LOCAL_VARIABLE(DrawingLogger, colorShadowColor2Vec.w);
		LOG_LOCAL_DEBUG(DrawingLogger, "Red, Green, Blue, Alpha");
		//First, we change from float ImVec4 to ImU32 (__int32)
		ImU32 ColorToWrite = ImGui::ColorConvertFloat4ToU32(colorShadowColor2Vec);
		//Then, we change BGRA to RGBA
		ColorToWrite = ColorsTools::SwapRBChannels(ColorToWrite);
		PlayableCharactersManager::ChangeOptionPaletteColor(ColorToWrite, ColorOptionFlag::FLAG_SUPER_SHADOW_2);
	}

	ImGui::Spacing();
	ImGui::Text("Colors Palette: %d", Curent_Char.Num_Of_Color);
	if (GroupColorManager::GetInstance().HasData() and bDrawColorGroup){
	
		GroupColors();
	}
	else {
		UnGroupColors();
	}


	
}

void Drawing::GroupColors() {
	auto& colors = Curent_Char.Character_Colors;
	if (auto groupsPtr = GroupColorManager::GetInstance().GetGroupsForCharacter(Curent_Char.Char_Name)) {
		const auto& groups = *groupsPtr;

		// Отображаем с группировкой
		for (const auto& group : groups) {
			// Добавляем ImGuiTreeNodeFlags_DefaultOpen для открытого состояния по умолчанию
			if (ImGui::CollapsingHeader(group.groupName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				// Используем стиль без отступов для более плотного расположения
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

				// Отображаем цвета в группе
				for (int i = group.startIndex;
					i < group.startIndex + group.count && i < colors.size();
					i++) {

					ImGui::PushID(i);
					//First of all, we change BGRA to RGBA
					ImU32 colorRGBA_U32 = ColorsTools::SwapRBChannels(colors[i]);
					//Then, we change ImU32 to ImVec4, for ColorEdit4 correct work
					ImVec4 colorVec = ImGui::ColorConvertU32ToFloat4(colorRGBA_U32);

					// Кнопка цвета
					if (ImGuiCustom::ColorEdit4(
						("##color_" + std::to_string(i)).c_str(),
						&colorVec.x, //Pass the address of the first element (x) to a function expecting float[4]
						ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar
					))
					{
						LOG_LOCAL_DEBUG(DrawingLogger, "Change color at index: ");
						LOG_LOCAL_VARIABLE(DrawingLogger, i);
						LOG_LOCAL_DEBUG(DrawingLogger, "Change color to: ");
						LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.x);
						LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.y);
						LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.z);
						LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.w);
						LOG_LOCAL_DEBUG(DrawingLogger, "Red, Green, Blue, Alpha");
						//First, we change from float ImVec4 to ImU32 (__int32)
						ImU32 ColorToWrite = ImGui::ColorConvertFloat4ToU32(colorVec);
						//Then, we change BGRA to RGBA
						ColorToWrite = ColorsTools::SwapRBChannels(ColorToWrite);
						PlayableCharactersManager::ChangePaletteColor(i, ColorToWrite);
					}


					ImGui::PopID();

					// Используем SameLine() с проверкой, помещается ли следующий элемент
					bool isLastInGroup = (i == group.startIndex + group.count - 1);
					bool isLastValidColor = (i == colors.size() - 1);

					if (!isLastInGroup && !isLastValidColor) {
						ImGui::SameLine();
					}
				}
				ImGui::PopStyleVar();
			}
		}
	}
	else {
		LOG_LOCAL_ERROR(DrawingLogger, "Can't find JSON for Character");
		UnGroupColors();
	}
}
void Drawing::UnGroupColors() {
	auto& colors = Curent_Char.Character_Colors;
	// Автоматический расчет столбцов
	float availableWidth = ImGui::GetContentRegionAvail().x;
	float colorButtonSize = ImGui::GetFrameHeight();
	float itemSpacing = ImGui::GetStyle().ItemSpacing.x;

	int colorsPerRow = (int)(availableWidth / (colorButtonSize + itemSpacing));
	if (colorsPerRow < 1) colorsPerRow = 1;

	// Счетчик для текущей строки
	int currentColumn = 0;

	for (int i = 1; i < colors.size(); i++) { //We start from second (1) color, becouse first color doesn't affect anything
		ImGui::PushID(i);
		//First of all, we change BGRA to RGBA
		ImU32 colorRGBA_U32 = ColorsTools::SwapRBChannels(colors[i]);
		//Then, we change ImU32 to ImVec4, for ColorEdit4 correct work
		ImVec4 colorVec = ImGui::ColorConvertU32ToFloat4(colorRGBA_U32);

		// Кнопка цвета
		if (ImGuiCustom::ColorEdit4(
			("##color_" + std::to_string(i)).c_str(),
			&colorVec.x, //Pass the address of the first element (x) to a function expecting float[4]
			ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar
		))
		{
			LOG_LOCAL_DEBUG(DrawingLogger, "Change color at index: ");
			LOG_LOCAL_VARIABLE(DrawingLogger, i);
			LOG_LOCAL_DEBUG(DrawingLogger, "Change color to: ");
			LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.x);
			LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.y);
			LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.z);
			LOG_LOCAL_VARIABLE(DrawingLogger, colorVec.w);
			LOG_LOCAL_DEBUG(DrawingLogger, "Red, Green, Blue, Alpha");
			//First, we change from float ImVec4 to ImU32 (__int32)
			ImU32 ColorToWrite = ImGui::ColorConvertFloat4ToU32(colorVec);
			//Then, we change BGRA to RGBA
			ColorToWrite = ColorsTools::SwapRBChannels(ColorToWrite);
			PlayableCharactersManager::ChangePaletteColor(i, ColorToWrite);
		}


		ImGui::PopID();

		// Решаем, добавлять SameLine или переходить на новую строку
		currentColumn++;
		if (currentColumn < colorsPerRow && i < colors.size() - 1) {
			ImGui::SameLine(0, itemSpacing);
		}
		else {
			currentColumn = 0;
		}
	}
}

void Drawing::DrawAutoLoadPaletteTabItem() {

	if (ImGui::Button("Add new Auto Load Pallete")) {
		AutoPalette::Auto_Pals.push_back(Auto_Pal{ "Filia", 0, "" });
		AutoPalette::save(); //We save new AutoPal
	}
	if (ImGui::Button("Reset Auto Palettes")) {
		AutoPalette::init();
	}
	ImGui::Separator();

	for (int i = 0; i < AutoPalette::Auto_Pals.size(); i++) {
		Auto_Pal& pal = AutoPalette::Auto_Pals[i];

		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.3f, 0.6f, 0.9f));

		// Группируем каждый элемент в Child с адаптивной высотой
		std::string childId = "AutoPal_group_" + std::to_string(i);

		// Используем флаг для авто-размера вместо фиксированной высоты
		ImGui::BeginChild(childId.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding, 0);

		// === Первая строка: Выбор персонажа и номер палитры ===
		
		ImGui::BeginGroup();
		{
			// Левая часть: Название + Combo
			ImGui::BeginGroup();
			ImGui::Text("Character Name");
			ImGui::SameLine();

			const char* preview_value = pal.CharName.c_str();
			std::string comboId = "##CharacterName_" + std::to_string(i);

			// Автоматическая ширина для Combo (50% от доступного пространства)
			float comboWidth = ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 2;
			comboWidth = std::max(comboWidth, 100.0f); // Минимальная ширина
			ImGui::SetNextItemWidth(comboWidth);

			if (ImGui::BeginCombo(comboId.c_str(), preview_value)) {
				for (int j = 0; j < IM_ARRAYSIZE(PlayableCharacterNames); j++) {
					bool isSelected = (pal.CharName == PlayableCharacterNames[j]);
					if (ImGui::Selectable(PlayableCharacterNames[j], isSelected)) {
						pal.CharName = PlayableCharacterNames[j];
						AutoPalette::save();
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::EndGroup();

			// Правая часть: Номер палитры
			ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x * 2); // Больший отступ

			ImGui::BeginGroup();
			ImGui::Text("Palette #");
			ImGui::SameLine();

			std::string palNumId = "##PalNum_" + std::to_string(i);
			int displayValue = pal.PalNum + 1;

			// Фиксированная ширина для InputInt (ширина для 3-4 цифр)
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("00000000").x + ImGui::GetStyle().FramePadding.x * 2);
			if (ImGui::InputInt(palNumId.c_str(), &displayValue)) {
				if (displayValue < 1) displayValue = 1;
				pal.PalNum = displayValue - 1;
				AutoPalette::save();
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();

		ImGui::Spacing(); // Отступ между строками

		// === Вторая строка: Путь к палитре ===
		ImGui::BeginGroup();
		{
			// Текст слева
			ImGui::Text("Path to the Palette");
			ImGui::SameLine();

			// Группа для InputText и кнопки
			ImGui::BeginGroup();
			{
				char pathBuffer[512];
				strncpy_s(pathBuffer, pal.PalPath.c_str(), sizeof(pathBuffer));
				pathBuffer[sizeof(pathBuffer) - 1] = '\0';
				std::string pathId = "##Path_" + std::to_string(i);

				// InputText занимает всё доступное пространство минус кнопка
				float availableWidth = ImGui::GetContentRegionAvail().x;
				float buttonWidth = ImGui::CalcTextSize("Open").x + ImGui::GetStyle().FramePadding.x * 2;
				float inputWidth = availableWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

				ImGui::PushItemWidth(inputWidth);
				if (ImGui::InputText(pathId.c_str(), pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ElideLeft)) {
					pal.PalPath = pathBuffer;
					AutoPalette::save();
				}
				ImGui::PopItemWidth();

				// Кнопка "Open" на той же строке
				ImGui::SameLine();
				if (ImGui::Button("Open")) {
					IGFD::FileDialogConfig config;
					config.path = ".";
					config.userDatas = reinterpret_cast<void*>(&pal);
					ImGuiFileDialog::Instance()->OpenDialog("LoadPaletteFile_AutoLoad", "Load Palette File for Auto Loading", ".pal", config);
				}
			}
			ImGui::EndGroup();
		}
		ImGui::EndGroup();

		ImGui::Spacing(); // Отступ перед разделителем

		ImGui::Separator();

		ImGui::Spacing(); // Отступ после разделителя

		// === Третья строка: Кнопка Delete ===
		// Центрируем кнопку Delete

		
		if (ImGui::Button("Delete")) {
			AutoPalette::Auto_Pals.erase(AutoPalette::Auto_Pals.begin() + i);
			AutoPalette::save();
		}

		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopID();

		//// Отступ между элементами списка
		//if (i < AutoPallete::Auto_Pals.size() - 1) {
		//	ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
		//}
	}

}


void Drawing::FileDialog() {
	if (ImGuiFileDialog::Instance()->Display("SavePaletteFile", ImGuiWindowFlags_NoDocking)) {

		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			PalleteFile::SaveToFile(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadPaletteFile", ImGuiWindowFlags_NoDocking)) {

		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			PalleteFile::LoadFromFile(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadJSON", ImGuiWindowFlags_NoDocking)) {

		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			GroupColorManager::GetInstance().LoadFromFile(filePathName);
			config::set_string("CharPart", filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGuiFileDialog::Instance()->Display("LoadPaletteFile_AutoLoad", ImGuiWindowFlags_NoDocking)) {

		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			Auto_Pal* palPtr = reinterpret_cast<Auto_Pal*>(ImGuiFileDialog::Instance()->GetUserDatas());
			palPtr->PalPath = filePathName;

			AutoPalette::save();
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}
