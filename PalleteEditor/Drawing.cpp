#include "Drawing.h"
#include "PalleteEditor.h"
#include "FileLoad.h"
#include "Auto-Load-Pallete.h"
#include "tinyfiledialogs.h"

void Drawing::Active()
{
	bDraw = true;
}

bool Drawing::isActive()
{
	return bDraw == true;
}

void Drawing::Draw()
{
	if (isActive())
	{
		PalEdit::Init();
		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::Begin(lpWindowName, &bDraw, WindowFlags);
		{
			//Меню-Бар
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Load Table"))
					{
						AddressTable::LoadFromFile();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Load JSON (Characters parts)"))
					{
						GroupColorGroup::LoadFromFile();
					}
					if (PalEdit::bGameOpenned and PalEdit::bMatchStarted and (PalEdit::current_character_idx != -1)) {
						ImGui::Separator();
						if (ImGui::MenuItem("Save Pallete"))
						{
							PalleteFile::SaveToFile(
								PalEdit::Character_Vector[PalEdit::FindVectorIndexByID(PalEdit::current_character_idx)]
							);
						}
						if (ImGui::MenuItem("Load Pallete"))
						{
							if (PalleteFile::LoadFromFile(
								PalEdit::Character_Vector[PalEdit::FindVectorIndexByID(PalEdit::current_character_idx)]
							)) {
								PalEdit::ChangeAllColors();
								PalEdit::ChangeLineColor();
								PalEdit::ChangeSuperShadow1();
								PalEdit::ChangeSuperShadow2();
								PalEdit::Read_Character();
							}
						}
					}
					ImGui::EndMenu();
	
				}

				// Кнопка About прямо в меню баре
				if (ImGui::MenuItem("About"))
				{
					// Открываем окно About при нажатии
					bShow_about_window = true;
				}

				ImGui::EndMenuBar();
			}
				//Окно "About"
				if (bShow_about_window)
				{
					ImGui::Begin("About", &bShow_about_window);
					ImGui::Text("Skullgirls Pallete Editor 2nd Enocore");
					ImGui::TextDisabled("Version: v0.4 ");
					ImGui::Text("Also, check our Discord: ");
					ImGui::TextLinkOpenURL("Discord", "https://discord.gg/4ufGJQjkpc");
					ImGui::Separator();
					if (ImGui::Button("OK"))
					{
						bShow_about_window = false;
					}
					ImGui::End();
				}
				if (ImGui::BeginTabBar("##TabBar")) {
				if (ImGui::BeginTabItem("Pallete")) {
			
				if (PalEdit::bGameOpenned != true) {
					ImGui::Text("Open the game to use the editor.");

				}
				else {
					if (PalEdit::bMatchStarted != true) {
						ImGui::Text("Start a match to use the editor.\nIf this message persists, the table may be invalid.");
					}
					else {
						//Комбо-бокс выбора персонажа
						ImGui::Text("Select character:");
						const char* preview_text = "Select";
						for (const auto& character : PalEdit::Character_Vector) {
							if (character.ID == PalEdit::current_character_idx) {
								preview_text = character.Char_Name.c_str();
								break;
							}
						}

						if (ImGui::BeginCombo("##CharSelect", preview_text)) {
							for (int i = 0; i < PalEdit::Character_Vector.size(); i++) {
								// Сравниваем ID, а не индекс в векторе
								bool is_selected = (PalEdit::current_character_idx == PalEdit::Character_Vector[i].ID);

								std::string Display_Name = PalEdit::Character_Vector[i].Char_Name;
								if (PalEdit::Character_Vector[i].ID < 3) {
									Display_Name += " (Player 1)";
								}
								else {
									Display_Name += " (Player 2)";
								}

								if (ImGui::Selectable(Display_Name.c_str(), is_selected)) {
									// Сохраняем ID выбранного персонажа
									PalEdit::current_character_idx = PalEdit::Character_Vector[i].ID;
									PalEdit::Read_Character();
								}
								if (is_selected) {
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}						
						if (PalEdit::current_character_idx != -1) {
							Character& currentChar = PalEdit::Character_Vector[PalEdit::FindVectorIndexByID(PalEdit::current_character_idx)];
							int displayValue = currentChar.Current_Pallete_Num + 1;
							if (ImGui::SliderInt("PalleteNum##", &displayValue, 1, currentChar.Max_Pallete_Num)) { //Слайдер выбора палетки
								{

									currentChar.Current_Pallete_Num = displayValue - 1;
									PalEdit::ChangePallete();
									PalEdit::Read_Character();
								};
							};
							ImGui::Separator();

							// Предварительные вычисления цветов (остаются без изменений)
							__int32& LineColor = currentChar.LineColor;
							float LinecolorFloat[4] = {
								((LineColor >> 16) & 0xFF) / 255.0f,
								((LineColor >> 8) & 0xFF) / 255.0f,
								(LineColor & 0xFF) / 255.0f,
								((LineColor >> 24) & 0xFF) / 255.0f
							};

							__int32& i32SuperShadow1 = currentChar.SuperShadowColor1;
							float fSuperShadow1[4] = {
								((i32SuperShadow1 >> 16) & 0xFF) / 255.0f,
								((i32SuperShadow1 >> 8) & 0xFF) / 255.0f,
								(i32SuperShadow1 & 0xFF) / 255.0f,
								((i32SuperShadow1 >> 24) & 0xFF) / 255.0f
							};

							__int32& i32SuperShadow2 = currentChar.SuperShadowColor2;
							float fSuperShadow2[4] = {
								((i32SuperShadow2 >> 16) & 0xFF) / 255.0f,
								((i32SuperShadow2 >> 8) & 0xFF) / 255.0f,
								(i32SuperShadow2 & 0xFF) / 255.0f,
								((i32SuperShadow2 >> 24) & 0xFF) / 255.0f
							};

							// Создаем таблицу с 2 колонками
							if (ImGui::BeginTable("ColorSettings", 2, ImGuiTableFlags_SizingFixedFit))
							{
								ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthFixed, 200.0f);
								ImGui::TableSetupColumn("Colors", ImGuiTableColumnFlags_WidthFixed, 150.0f);

								// Первая строка: Don't display character
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (ImGui::Checkbox("Don't display character", &PalEdit::bNODisplayChar)) {
									PalEdit::NODisplayChar();
								}

								ImGui::TableSetColumnIndex(1);
								if (ImGui::ColorEdit3("Line Color", LinecolorFloat, ImGuiColorEditFlags_NoInputs)) {
									__int32 LineColorValue =
										(static_cast<__int32>(LinecolorFloat[3] * 255) << 24) |
										(static_cast<__int32>(LinecolorFloat[0] * 255) << 16) |
										(static_cast<__int32>(LinecolorFloat[1] * 255) << 8) |
										(static_cast<__int32>(LinecolorFloat[2] * 255));

									currentChar.LineColor = LineColorValue;
									PalEdit::ChangeLineColor();
									PalEdit::Read_Character();
								}

								// Вторая строка: Don't display shadows
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (ImGui::Checkbox("Don't display shadows", &PalEdit::bNODisplayShadows)) {
									PalEdit::NODisplayShadow();
								}

								ImGui::TableSetColumnIndex(1);
								if (ImGui::ColorEdit3("Super Shadow 1", fSuperShadow1, ImGuiColorEditFlags_NoInputs)) {
									__int32 ColorEdit =
										(static_cast<__int32>(fSuperShadow1[3] * 255) << 24) |
										(static_cast<__int32>(fSuperShadow1[0] * 255) << 16) |
										(static_cast<__int32>(fSuperShadow1[1] * 255) << 8) |
										(static_cast<__int32>(fSuperShadow1[2] * 255));

									currentChar.SuperShadowColor1 = ColorEdit;
									PalEdit::ChangeSuperShadow1();
									PalEdit::Read_Character();
								}

								// Третья строка: Display super shadow
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (ImGui::Checkbox("Display super shadow", &PalEdit::bDisplaySuperShadows)) {
									PalEdit::DisplaySuperShadow();
								}

								ImGui::TableSetColumnIndex(1);
								if (ImGui::ColorEdit3("Super Shadow 2", fSuperShadow2, ImGuiColorEditFlags_NoInputs)) {
									__int32 ColorEdit =
										(static_cast<__int32>(fSuperShadow2[3] * 255) << 24) |
										(static_cast<__int32>(fSuperShadow2[0] * 255) << 16) |
										(static_cast<__int32>(fSuperShadow2[1] * 255) << 8) |
										(static_cast<__int32>(fSuperShadow2[2] * 255));

									currentChar.SuperShadowColor2 = ColorEdit;
									PalEdit::ChangeSuperShadow2();
									PalEdit::Read_Character();
								}
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								auto it = GroupColorGroup::characterGroups.find(currentChar.Char_Name); //Если есть JSON с Частями персонжей
								if (it != GroupColorGroup::characterGroups.end() && !it->second.empty()) {
									bJSONEnable = true;
									ImGui::Checkbox("Group Character Parts", &bGrouping);
								}
								ImGui::EndTable();
							}


							ImGui::Text("Color Palettes: %d", currentChar.Num_Of_Color);
							ImGui::Separator();
							if (bJSONEnable and bGrouping) {
								auto it = GroupColorGroup::characterGroups.find(currentChar.Char_Name);
								// Отображаем с группировкой
								for (const auto& group : it->second) {
									// Добавляем ImGuiTreeNodeFlags_DefaultOpen для открытого состояния по умолчанию
									if (ImGui::CollapsingHeader(group.groupName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
										// Используем стиль без отступов для более плотного расположения
										ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

										// Отображаем цвета в группе
										for (int i = group.startIndex;
											i < group.startIndex + group.count && i < currentChar.Character_Colors.size();
											i++) {

											__int32& colorValue = currentChar.Character_Colors[i];

											float colorFloat[4] = {
												((colorValue >> 16) & 0xFF) / 255.0f,
												((colorValue >> 8) & 0xFF) / 255.0f,
												(colorValue & 0xFF) / 255.0f,
												((colorValue >> 24) & 0xFF) / 255.0f
											};

											ImGui::PushID(i);
											if (ImGui::ColorEdit4(("##Color_" + std::to_string(i)).c_str(),
												colorFloat,
												ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar)) {
												colorValue =
													(static_cast<__int32>(colorFloat[3] * 255) << 24) |
													(static_cast<__int32>(colorFloat[0] * 255) << 16) |
													(static_cast<__int32>(colorFloat[1] * 255) << 8) |
													(static_cast<__int32>(colorFloat[2] * 255));

												PalEdit::ChangeColor(i, colorValue);
												PalEdit::Read_Character();
											}

											if (ImGui::IsItemHovered()) {
												ImGui::SetTooltip("Index: %d", i);
											}

											ImGui::PopID();

											// Используем SameLine() с проверкой, помещается ли следующий элемент
											bool isLastInGroup = (i == group.startIndex + group.count - 1);
											bool isLastValidColor = (i == currentChar.Character_Colors.size() - 1);

											if (!isLastInGroup && !isLastValidColor) {
												ImGui::SameLine();
											}
										}

										ImGui::PopStyleVar();
									}
								}
							} //Если JSON нет.
							else {
								for (int i = 1; i < currentChar.Character_Colors.size(); i++) {
									// Получаем ссылку на 32-битный цвет
									__int32& colorValue = currentChar.Character_Colors[i];

									// Распаковываем компоненты (формат ARGB)
									float colorFloat[4] = {
										((colorValue >> 16) & 0xFF) / 255.0f,  // Red
										((colorValue >> 8) & 0xFF) / 255.0f,   // Green
										(colorValue & 0xFF) / 255.0f,          // Blue
										((colorValue >> 24) & 0xFF) / 255.0f   // Alpha
									};

									ImGui::PushID(i);
									// Отображаем ColorEdit
									if (ImGui::ColorEdit4(("Color##" + std::to_string(i)).c_str(), colorFloat, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar)) {

										// Упаковываем обратно в ARGB формат
										colorValue =
											(static_cast<__int32>(colorFloat[3] * 255) << 24) |  // Alpha
											(static_cast<__int32>(colorFloat[0] * 255) << 16) |  // Red
											(static_cast<__int32>(colorFloat[1] * 255) << 8) |   // Green
											(static_cast<__int32>(colorFloat[2] * 255));         // Blue

										PalEdit::ChangeColor(i, colorValue);
										PalEdit::Read_Character();
									}

									ImGui::PopID();

									if (i > 0 and i % 16 != 0) {
										ImGui::SameLine();
									}
								}
							}

						}




				}
			}
			ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Auto Load Pallete")) {
						if (ImGui::Button("Add new Auto Load Pallete")) {
							AutoPallete::Auto_Pals.push_back(Auto_Pal{ "Filia", 0, "" });
							AutoPallete::save(); // Добавлено
						}
						if (ImGui::Button("Reset Auto Palettes")) {
							AutoPallete::init();
						}
						ImGui::Separator();

						for (int i = 0; i < AutoPallete::Auto_Pals.size(); i++) {
							Auto_Pal& pal = AutoPallete::Auto_Pals[i];
							ImGui::PushID(i);
							ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.3f, 0.6f, 0.9f)); // синий фон
							ImGui::BeginChild(ImGui::GetID("pal_group"), ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 4), true, ImGuiWindowFlags_NoDecoration);

							// Создаём таблицу с 3 колонками
							ImGui::BeginTable("ColorSettings", 3, ImGuiTableFlags_SizingFixedFit);

							// Настраиваем колонки
							ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, 150.0f);  // Метки
							ImGui::TableSetupColumn("Inputs", ImGuiTableColumnFlags_WidthStretch);        // Текстовые поля
							ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 175.0f); // Кнопки и числа

							// Первая строка: Character Name
							ImGui::TableNextRow();

							// Первая колонка: метка
							ImGui::TableSetColumnIndex(0);
							ImGui::AlignTextToFramePadding();
							ImGui::Text("Character Name");

							// Вторая колонка: текстовое поле
							ImGui::TableSetColumnIndex(1);
							char charNameBuffer[256];
							strncpy_s(charNameBuffer, pal.CharName.c_str(), sizeof(charNameBuffer));
							charNameBuffer[sizeof(charNameBuffer) - 1] = '\0';
							ImGui::SetNextItemWidth(-FLT_MIN); // Занимает оставшееся пространство
							const char* preview_value = pal.CharName.c_str();

							if (ImGui::BeginCombo("##CharacterName", preview_value)) {
								for (int j = 0; j < IM_ARRAYSIZE(characterNames); j++) {
									bool isSelected = (pal.CharName == characterNames[j]);
									if (ImGui::Selectable(characterNames[j], isSelected)) {
										pal.CharName = characterNames[j];
										AutoPallete::save(); // Добавлено
									}
									if (isSelected) {
										ImGui::SetItemDefaultFocus();
									}
								}
								ImGui::EndCombo();
							}

							// Третья колонка: Label + InputInt
							ImGui::TableSetColumnIndex(2);
							ImGui::AlignTextToFramePadding();
							ImGui::Text("Palette Number");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(-FLT_MIN); // Занимает оставшееся пространство
							int displayValue = pal.PalNum + 1;
							if (ImGui::InputInt("##PalNum", &displayValue))
							{
								// Ограничиваем минимальное значение 1
								if (displayValue < 1) displayValue = 1;
								pal.PalNum = displayValue - 1;
								AutoPallete::save(); // Добавлено
							}

							// Вторая строка: Path
							ImGui::TableNextRow();

							// Первая колонка: метка
							ImGui::TableSetColumnIndex(0);
							ImGui::AlignTextToFramePadding();
							ImGui::Text("Path to the Palette");

							// Вторая колонка: текстовое поле
							ImGui::TableSetColumnIndex(1);
							static char pathBuffer[512]; // Убрана статическая для каждого элемента
							strncpy_s(pathBuffer, pal.PalPath.c_str(), sizeof(pathBuffer));
							pathBuffer[sizeof(pathBuffer) - 1] = '\0';
							ImGui::SetNextItemWidth(-FLT_MIN); // Занимает оставшееся пространство
							if (ImGui::InputText("##Path", pathBuffer, sizeof(pathBuffer))) {
								pal.PalPath = pathBuffer;
								AutoPallete::save(); // Добавлено
							}

							// Третья колонка: кнопка Open
							ImGui::TableSetColumnIndex(2);
							if (ImGui::Button("Open", ImVec2(-FLT_MIN, 0))) {
								// Код для открытия
								const char* filterPatterns[1] = { "*.pal" };
								const char* filePath = tinyfd_openFileDialog(
									"Load Pallete",        // заголовок
									"",                     // начальная директория
									1,                      // количество фильтров
									filterPatterns,         // фильтры
									NULL,                   // описание фильтров
									0                       // множественный выбор (0 - нет, 1 - да)
								);
								if (filePath != NULL) {
									pal.PalPath = filePath;
									AutoPallete::save(); // Добавлено
								}
							}

							ImGui::EndTable();

							ImGui::Separator();

							// Кнопка Delete под таблицей
							if (ImGui::Button("Delete")) {
								AutoPallete::Auto_Pals.erase(AutoPallete::Auto_Pals.begin() + i);
								AutoPallete::save(); // Добавлено
								// После удаления нужно выйти из цикла, так как индексы изменились
								ImGui::EndChild();
								ImGui::PopStyleColor();
								ImGui::PopID();
								break;
							}

							ImGui::EndChild();
							ImGui::PopStyleColor();
							ImGui::PopID();
						}

						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
				ImGui::End();
		}
	}
}
