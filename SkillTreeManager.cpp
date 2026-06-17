#include "SkillTreeManager.h"
#include "imgui.h"

// ===================================================
// コンストラクタ
// ===================================================
SkillTreeManager::SkillTreeManager() : currentCategoryIndex(0), playerSP(10), playerAttackPower(10.0f), playerMaxHP(100.0f), playerMoveSpeed(2.0f) {}

// ===================================================
// スキルツリーのデータ初期化
// ===================================================
void SkillTreeManager::Initialize() {
	categories.clear();

	// ---------------------------------------------------
	// ステータスカテゴリーの作成とスキル登録
	// ---------------------------------------------------
	SkillCategory statusCategory("ステータス");

	// --- 腕立て伏せルート：1層目および2層目 ---
	statusCategory.AddSkill(Skill(1, "腕立て伏せ", "基礎的な筋力を鍛えます。", 1, 1, 300.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(2, "物理攻撃アップ", "攻撃力が上昇します。", 5, 2, 200.0f, 400.0f, 1));
	statusCategory.AddSkill(Skill(3, "最大HPアップ(腕)", "最大HPが上昇します。", 5, 2, 400.0f, 400.0f, 1));

	// --- 腕立て伏せルート：3層目の派生スキル ---
	// 物理攻撃アップ(ID:2) からの分岐
	statusCategory.AddSkill(Skill(7, "超・力溜め", "一時的に攻撃力を大増幅します。", 3, 4, 30.0f, 400.0f, 2));
	statusCategory.AddSkill(Skill(11, "背水之陣", "防御を捨てて攻撃力を極限まで高めます。", 3, 4, 170.0f, 400.0f, 2));

	// 最大HPアップ(腕)(ID:3) からの分岐
	statusCategory.AddSkill(Skill(12, "不屈の精神", "倒れにくくなり、粘り強く戦えます。", 3, 4, 270.0f, 400.0f, 3));
	statusCategory.AddSkill(Skill(8, "金剛不壊", "肉体を硬化させ防御を固めます。", 3, 4, 410.0f, 400.0f, 3));

	// --- スクワットルート：1層目および2層目 ---
	statusCategory.AddSkill(Skill(4, "スクワット", "下半身の基礎体力を鍛えます。", 1, 1, 800.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(5, "素早さアップ", "移動速度が上昇します。", 5, 2, 700.0f, 400.0f, 4));
	statusCategory.AddSkill(Skill(6, "最大HPアップ(脚)", "最大HPが上昇します。", 5, 2, 900.0f, 400.0f, 4));

	// --- スクワットルート：3層目の派生スキル ---
	// 素早さアップ(ID:5) からの分岐
	statusCategory.AddSkill(Skill(9, "神速", "残像が生じるほどの瞬発力を得ます。", 3, 4, 590.0f, 400.0f, 5));
	statusCategory.AddSkill(Skill(13, "縮地法", "一瞬で間合いを詰める移動術です。", 3, 4, 730.0f, 400.0f, 5));

	// 最大HPアップ(脚)(ID:6) からの分岐
	statusCategory.AddSkill(Skill(14, "鋼鉄の足腰", "下半身の強靭さを極め、怯みにくくなります。", 3, 4, 830.0f, 400.0f, 6));
	statusCategory.AddSkill(Skill(10, "金剛脚", "強烈な蹴り技を可能にします。", 3, 4, 970.0f, 400.0f, 6));

	categories.push_back(statusCategory);

	// ---------------------------------------------------
	// 各種カテゴリーの追加設定
	// ---------------------------------------------------
	SkillCategory taigiCategory("体技");
	taigiCategory.AddSkill(Skill(101, "体当たり", "敵に突進してダメージを与えます。", 1, 2, 550.0f, 200.0f, -1));
	categories.push_back(taigiCategory);

	SkillCategory jmonCategory("呪文");
	categories.push_back(jmonCategory);

	SkillCategory tansakuCategory("探索");
	categories.push_back(tansakuCategory);
}

// ===================================================
// スキル習得時のステータス上昇処理
// ===================================================
void SkillTreeManager::ApplySkillEffect(int skillId) {
	switch (skillId) {
	case 1:
		playerAttackPower += 1.0f;
		break;
	case 2:
		playerAttackPower += 5.0f;
		break;
	case 3:
	case 6:
		playerMaxHP += 20.0f;
		break;
	case 4:
		playerMoveSpeed += 0.2f;
		break;
	case 5:
		playerMoveSpeed += 0.5f;
		break;
	case 7:
		playerAttackPower += 15.0f;
		break; // 超・力溜め
	case 8:
		playerMaxHP += 50.0f;
		break; // 金剛不壊
	case 9:
		playerMoveSpeed += 1.2f;
		break; // 神速
	case 10:
		playerAttackPower += 10.0f;
		break; // 金剛脚

	// 追加スキルのステータス上昇効果
	case 11:
		playerAttackPower += 25.0f;
		break; // 背水之陣
	case 12:
		playerMaxHP += 40.0f;
		break; // 不屈の精神
	case 13:
		playerMoveSpeed += 0.8f;
		break; // 縮地法
	case 14:
		playerMaxHP += 30.0f;
		break; // 鋼鉄の足腰

	default:
		break;
	}
}

// ===================================================
// スキルリセットおよびSP返還処理
// ===================================================
void SkillTreeManager::ResetAllSkills() {
	// プレイヤーパラメータを初期値に再設定
	playerAttackPower = 10.0f;
	playerMaxHP = 100.0f;
	playerMoveSpeed = 2.0f;

	// 全スキルデータの走査と初期化
	for (auto& category : categories) {
		for (auto& skill : category.GetSkills()) {
			// 習得済みスキルから消費SPを算出して返還
			if (skill.currentLv > 0) {
				playerSP += (skill.requiredSP * skill.currentLv);
				skill.currentLv = 0;
			}

			// 前提条件に応じた解放状態の再構築
			if (skill.parentSkillId == -1) {
				skill.state = SkillState::Available;
			} else {
				skill.state = SkillState::Locked;
			}
		}
		category.UpdateSkillStates();
	}
}

// ===================================================
// 更新処理
// ===================================================
void SkillTreeManager::Update(float mouseX, float mouseY, bool isMouseClicked) {
	if (categories.empty())
		return;

	auto& currentCategory = categories[currentCategoryIndex];

	// 入力検知時のスキル習得判定処理
	if (isMouseClicked) {
		for (auto& skill : currentCategory.GetSkills()) {
			if (skill.IsMouseOver(mouseX, mouseY) && skill.state == SkillState::Available) {
				if (playerSP >= skill.requiredSP) {
					playerSP -= skill.requiredSP;
					skill.LevelUp();
					ApplySkillEffect(skill.id);
					currentCategory.UpdateSkillStates();
				}
			}
		}
	}
}

// ===================================================
// 描画処理
// ===================================================
void SkillTreeManager::Draw() {
	ImGui::Begin("スキルツリー 開発画面", nullptr, ImGuiWindowFlags_NoScrollbar);

	// ---------------------------------------------------
	// 開発者用ステータス管理表示
	// ---------------------------------------------------
	ImGui::BeginChild("StatusFrame", ImVec2(0, 75), true);

	ImGui::Text("【 プレイヤー現在のステータス 】");
	ImGui::Text("所持SP: %d", playerSP);
	ImGui::SameLine();
	ImGui::Text(" | 攻撃力: %.1f", playerAttackPower);
	ImGui::SameLine();
	ImGui::Text(" | 最大HP: %.1f", playerMaxHP);
	ImGui::SameLine();
	ImGui::Text(" | 移動速度: %.1f", playerMoveSpeed);

	// デバッグ用コントロール操作ボタン
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 340);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

	if (ImGui::Button("スキル全リセット", ImVec2(150, 25))) {
		ResetAllSkills();
	}

	ImGui::SameLine();

	if (ImGui::Button("レベルアップ (SP+3)", ImVec2(150, 25))) {
		playerSP += 3;
	}

	ImGui::EndChild();
	ImGui::Spacing();

	// ---------------------------------------------------
	// カテゴリー切り替えタブエリア
	// ---------------------------------------------------
	if (ImGui::BeginTabBar("CategoryTabBar")) {
		for (int i = 0; i < categories.size(); ++i) {
			if (ImGui::BeginTabItem(categories[i].GetName().c_str())) {
				currentCategoryIndex = i;

				ImGui::Spacing();
				ImGui::Text("--- %s ツリー ---", categories[i].GetName().c_str());
				ImGui::Spacing();

				// スクロール可能なノード配置キャンバスの開始
				ImGui::BeginChild("TreeCanvas", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 basePos = ImGui::GetCursorScreenPos();

				// ステータスカテゴリー選択時のみ接続線を描画
				if (categories[i].GetName() == "ステータス" && categories[currentCategoryIndex].GetName() == "ステータス") {
					ImU32 lineColor = IM_COL32(255, 255, 255, 255);
					float lineThickness = 2.0f;

					// -----------------------------------------------
					// 1層目から2層目を結ぶ接続座標の計算
					// -----------------------------------------------
					// 下層の広がりに合わせて、1層目と2層目の中心線を同期
					ImVec2 pos1 = ImVec2(basePos.x + 245.0f + 60.0f, basePos.y + 40.0f + 70.0f); // 腕立て伏せ
					ImVec2 pos2 = ImVec2(basePos.x + 115.0f + 60.0f, basePos.y + 215.0f);        // 物理攻撃アップ
					ImVec2 pos3 = ImVec2(basePos.x + 375.0f + 60.0f, basePos.y + 215.0f);        // 最大HP(腕)

					ImVec2 pos4 = ImVec2(basePos.x + 775.0f + 60.0f, basePos.y + 40.0f + 70.0f); // スクワット
					ImVec2 pos5 = ImVec2(basePos.x + 645.0f + 60.0f, basePos.y + 215.0f);        // 素早さアップ
					ImVec2 pos6 = ImVec2(basePos.x + 905.0f + 60.0f, basePos.y + 215.0f);        // 最大HP(脚)

					// -----------------------------------------------
					// 2層目から3層目を結ぶ接続座標の計算
					// -----------------------------------------------
					ImVec2 pos2_under = ImVec2(basePos.x + 115.0f + 60.0f, basePos.y + 220.0f + 70.0f);
					ImVec2 pos3_under = ImVec2(basePos.x + 375.0f + 60.0f, basePos.y + 220.0f + 70.0f);
					ImVec2 pos5_under = ImVec2(basePos.x + 645.0f + 60.0f, basePos.y + 220.0f + 70.0f);
					ImVec2 pos6_under = ImVec2(basePos.x + 905.0f + 60.0f, basePos.y + 220.0f + 70.0f);

					// 3層目：等間隔マージン（130px刻み）を基準とした配置座標
					ImVec2 pos7 = ImVec2(basePos.x + 50.0f + 60.0f, basePos.y + 395.0f);   // 超・力溜め
					ImVec2 pos11 = ImVec2(basePos.x + 180.0f + 60.0f, basePos.y + 395.0f); // 背水之陣
					ImVec2 pos12 = ImVec2(basePos.x + 310.0f + 60.0f, basePos.y + 395.0f); // 不屈の精神
					ImVec2 pos8 = ImVec2(basePos.x + 440.0f + 60.0f, basePos.y + 395.0f);  // 金剛不壊

					ImVec2 pos9 = ImVec2(basePos.x + 580.0f + 60.0f, basePos.y + 395.0f);  // 神速
					ImVec2 pos13 = ImVec2(basePos.x + 710.0f + 60.0f, basePos.y + 395.0f); // 縮地法
					ImVec2 pos14 = ImVec2(basePos.x + 840.0f + 60.0f, basePos.y + 395.0f); // 鋼鉄の足腰
					ImVec2 pos10 = ImVec2(basePos.x + 970.0f + 60.0f, basePos.y + 395.0f); // 金剛脚

					// -----------------------------------------------
					// 構造の接続線描画（末広がり形式）
					// -----------------------------------------------
					// 1層目から2層目への接続
					drawList->AddLine(pos1, pos2, lineColor, lineThickness);
					drawList->AddLine(pos1, pos3, lineColor, lineThickness);
					drawList->AddLine(pos4, pos5, lineColor, lineThickness);
					drawList->AddLine(pos4, pos6, lineColor, lineThickness);

					// 2層目から3層目への接続（親ノードから各子ノードへの分岐）
					drawList->AddLine(pos2_under, pos7, lineColor, lineThickness);  // 物理攻撃 ➔ 超・力溜め
					drawList->AddLine(pos2_under, pos11, lineColor, lineThickness); // 物理攻撃 ➔ 背水之陣

					drawList->AddLine(pos3_under, pos12, lineColor, lineThickness); // HP(腕) ➔ 不屈の精神
					drawList->AddLine(pos3_under, pos8, lineColor, lineThickness);  // HP(腕) ➔ 金剛不壊

					drawList->AddLine(pos5_under, pos9, lineColor, lineThickness);  // 素早さ ➔ 神速
					drawList->AddLine(pos5_under, pos13, lineColor, lineThickness); // 素早さ ➔ 縮地法

					drawList->AddLine(pos6_under, pos14, lineColor, lineThickness); // HP(脚) ➔ 鋼鉄の足腰
					drawList->AddLine(pos6_under, pos10, lineColor, lineThickness); // HP(脚) ➔ 金剛脚
				}

				// ---------------------------------------------------
				// スキルノード群の個別描画
				// ---------------------------------------------------
				for (auto& skill : categories[i].GetSkills()) {

					float posX = 0.0f;
					float posY = 0.0f;

					switch (skill.id) {
					// 3層目の均等マージンを基準に、上層のX座標を逆算して配置

					// --- 1層目：ベーススキル ---
					case 1:
						posX = 245.0f;
						posY = 40.0f;
						break; // 腕立て伏せ
					case 4:
						posX = 775.0f;
						posY = 40.0f;
						break; // スクワット

					// --- 2層目：派生スキル ---
					case 2:
						posX = 115.0f;
						posY = 220.0f;
						break; // 物理攻撃アップ
					case 3:
						posX = 375.0f;
						posY = 220.0f;
						break; // 最大HPアップ(腕)
					case 5:
						posX = 645.0f;
						posY = 220.0f;
						break; // 素早さアップ
					case 6:
						posX = 905.0f;
						posY = 220.0f;
						break; // 最大HPアップ(脚)

					// --- 3層目：派生スキル（横一列への展開） ---
					case 7:
						posX = 50.0f;
						posY = 400.0f;
						break; // 超・力溜め
					case 11:
						posX = 180.0f;
						posY = 400.0f;
						break; // 背水之陣
					case 12:
						posX = 310.0f;
						posY = 400.0f;
						break; // 不屈の精神
					case 8:
						posX = 440.0f;
						posY = 400.0f;
						break; // 金剛不壊

					case 9:
						posX = 580.0f;
						posY = 400.0f;
						break; // 神速
					case 13:
						posX = 710.0f;
						posY = 400.0f;
						break; // 縮地法
					case 14:
						posX = 840.0f;
						posY = 400.0f;
						break; // 鋼鉄の足腰
					case 10:
						posX = 970.0f;
						posY = 400.0f;
						break; // 金剛脚

					// --- 独立カテゴリースキル ---
					case 101:
						posX = 510.0f;
						posY = 130.0f;
						break; // 体当たり

					default:
						posX = 50.0f;
						posY = 50.0f;
						break;
					}

					// 指定の座標にカーソルを移動させ描画グループを開始
					ImGui::SetCursorPos(ImVec2(posX, posY));
					ImGui::BeginGroup();

					// スキル状態に基づいたカラー定義
					if (skill.state == SkillState::Locked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 灰色
					} else if (skill.state == SkillState::Available) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // 緑色
					}

					ImGui::Text("%s", skill.name.c_str());
					ImGui::PopStyleColor();

					// 進行度のプログレスバー表示
					float progress = (float)skill.currentLv / (float)skill.maxLv;
					std::string barLabel = std::to_string(skill.currentLv) + "/" + std::to_string(skill.maxLv);
					ImGui::ProgressBar(progress, ImVec2(120, 15), barLabel.c_str());

					// 状態に応じた操作インタラクションの描画
					if (skill.state == SkillState::Locked) {
						ImGui::TextDisabled("[ロック中]");
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::Text("[マスター]");
					} else {
						char buf[64];
						sprintf_s(buf, "習得 (SP:%d)##%d", skill.requiredSP, skill.id);
						if (ImGui::Button(buf, ImVec2(120, 22))) {
							if (playerSP >= skill.requiredSP) {
								playerSP -= skill.requiredSP;
								skill.LevelUp();
								ApplySkillEffect(skill.id);
								categories[i].UpdateSkillStates();
							}
						}
					}
					ImGui::EndGroup();
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}