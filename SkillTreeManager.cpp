#include "SkillTreeManager.h"
#include "imgui.h"

// ===================================================
// コンストラクタ
// ===================================================
SkillTreeManager::SkillTreeManager()
    : currentCategoryIndex(0), playerSP(10), playerAttackPower(10.0f), // 初期攻撃力
      playerMaxHP(100.0f),                                             // 初期最大HP
      playerMoveSpeed(2.0f)                                            // 初期移動速度
{}

// ===================================================
// スキルツリーのデータ初期化
// ===================================================
void SkillTreeManager::Initialize() {
	categories.clear();

	// ---------------------------------------------------
	// 1. 「ステータス」カテゴリーの作成
	// ---------------------------------------------------
	SkillCategory statusCategory("ステータス");

	// 第1ツリー：腕立て伏せルート
	statusCategory.AddSkill(Skill(1, "腕立て伏せ", "基礎的な筋力を鍛えます。", 1, 1, 300.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(2, "物理攻撃アップ", "攻撃力が上昇します。", 5, 2, 200.0f, 400.0f, 1));   // 前提:腕立て伏せ(ID:1)
	statusCategory.AddSkill(Skill(3, "最大HPアップ(腕)", "最大HPが上昇します。", 5, 2, 400.0f, 400.0f, 1)); // 前提:腕立て伏せ(ID:1)

	// 第2ツリー：スクワットルート
	statusCategory.AddSkill(Skill(4, "スクワット", "下半身の基礎体力を鍛えます。", 1, 1, 800.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(5, "素早さアップ", "移動速度が上昇します。", 5, 2, 700.0f, 400.0f, 4));   // 前提:スクワット(ID:4)
	statusCategory.AddSkill(Skill(6, "最大HPアップ(脚)", "最大HPが上昇します。", 5, 2, 900.0f, 400.0f, 4)); // 前提:スクワット(ID:4)

	categories.push_back(statusCategory);

	// ---------------------------------------------------
	// 2. 「体技」カテゴリーの作成
	// ---------------------------------------------------
	SkillCategory taigiCategory("体技");

	// 体当たりルート
	taigiCategory.AddSkill(Skill(101, "体当たり", "敵に突進してダメージを与えます。", 1, 2, 550.0f, 200.0f, -1));

	categories.push_back(taigiCategory);

	// ---------------------------------------------------
	// 3. 「呪文」カテゴリーの作成（枠だけ用意）
	// ---------------------------------------------------
	SkillCategory jmonCategory("呪文");
	categories.push_back(jmonCategory);

	// ---------------------------------------------------
	// 4. 「探索」カテゴリーの作成（枠だけ用意）
	// ---------------------------------------------------
	SkillCategory tansakuCategory("探索");
	categories.push_back(tansakuCategory);
}

// ===================================================
// スキルレベルアップ時のステータス反映処理（内部処理）
// ===================================================
void SkillTreeManager::ApplySkillEffect(int skillId) {
	// スキルのIDに応じてプレイヤーの能力値を上昇させる
	switch (skillId) {
	case 1: // 腕立て伏せ
		playerAttackPower += 1.0f;
		break;
	case 2: // 物理攻撃アップ
		playerAttackPower += 5.0f;
		break;
	case 3: // 最大HPアップ(腕)
	case 6: // 最大HPアップ(脚)
		playerMaxHP += 20.0f;
		break;
	case 4: // スクワット
		playerMoveSpeed += 0.2f;
		break;
	case 5: // 素早さアップ
		playerMoveSpeed += 0.5f;
		break;
	default:
		break;
	}
}

// ===================================================
// 更新処理
// ===================================================
void SkillTreeManager::Update(float mouseX, float mouseY, bool isMouseClicked) {
	if (categories.empty())
		return;
	auto& currentCategory = categories[currentCategoryIndex];

	if (isMouseClicked) {
		for (auto& skill : currentCategory.GetSkills()) {
			if (skill.IsMouseOver(mouseX, mouseY) && skill.state == SkillState::Available) {
				if (playerSP >= skill.requiredSP) {
					playerSP -= skill.requiredSP;
					skill.LevelUp();

					// スキルの効果をプレイヤーに適用
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
	// 【修正】これですべての要素がこの一つの大きなウィンドウの中に綺麗に収まります！
	ImGui::Begin("スキルツリー 開発画面", nullptr, ImGuiWindowFlags_NoScrollbar);

	// ---------------------------------------------------
	// 1. プレイヤーのステータス表示エリア（最上部に配置）
	// ---------------------------------------------------
	// 横幅はウィンドウ全体（0）、高さ75ピクセルの固定枠にします
	ImGui::BeginChild("StatusFrame", ImVec2(0, 75), true);

	ImGui::Text("【 プレイヤー現在のステータス 】");
	ImGui::Text("所持SP: %d", playerSP);
	ImGui::SameLine();
	ImGui::Text(" | 攻撃力: %.1f", playerAttackPower);
	ImGui::SameLine();
	ImGui::Text(" | 最大HP: %.1f", playerMaxHP);
	ImGui::SameLine();
	ImGui::Text(" | 移動速度: %.1f", playerMoveSpeed);

	// SP獲得（レベルアップ）ボタンを枠内の右端に配置
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 180);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
	if (ImGui::Button("レベルアップ (SP+3)", ImVec2(150, 25))) {
		playerSP += 3;
	}

	ImGui::EndChild(); // ステータス枠ここまで
	ImGui::Spacing();

	// ---------------------------------------------------
	// 2. カテゴリー（タブ）の切り替え部分
	// ---------------------------------------------------
	if (ImGui::BeginTabBar("CategoryTabBar")) {
		for (int i = 0; i < categories.size(); ++i) {
			if (ImGui::BeginTabItem(categories[i].GetName().c_str())) {
				currentCategoryIndex = i;

				ImGui::Spacing();
				ImGui::Text("--- %s ツリー ---", categories[i].GetName().c_str());
				ImGui::Spacing();

				// ---------------------------------------------------
				// 3. スキル表示用のキャンバス（タブの下にぴったり配置）
				// ---------------------------------------------------
				// 縦幅を「0」にすることで、ウィンドウの残りのスペース全体に自動で広がります
				ImGui::BeginChild("TreeCanvas", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

				// タブの中身（スキル一覧）を表示
				for (auto& skill : categories[i].GetSkills()) {

					// スキルのIDに応じて配置する座標（X, Y）を決める
					float posX = 0.0f;
					float posY = 0.0f;

					switch (skill.id) {
					// --- 腕立て伏せルート（左側） ---
					case 1:
						posX = 150.0f;
						posY = 40.0f;
						break;
					case 2:
						posX = 50.0f;
						posY = 220.0f;
						break;
					case 3:
						posX = 250.0f;
						posY = 220.0f;
						break;

					// --- スクワットルート（右側） ---
					case 4:
						posX = 650.0f;
						posY = 40.0f;
						break;
					case 5:
						posX = 550.0f;
						posY = 220.0f;
						break;
					case 6:
						posX = 750.0f;
						posY = 220.0f;
						break;

					// --- 体技：体当たり ---
					case 101:
						posX = 400.0f;
						posY = 130.0f;
						break;

					default:
						posX = 50.0f;
						posY = 50.0f;
						break;
					}

					// 指定した座標に描画位置を強制移動させる
					ImGui::SetCursorPos(ImVec2(posX, posY));

					// 1つのスキルを四角いグループ（ノード）としてまとめる
					ImGui::BeginGroup();

					// スキルの状態によって文字の色を変える
					if (skill.state == SkillState::Locked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 灰色
					} else if (skill.state == SkillState::Available) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // 緑色
					}

					// スキル名
					ImGui::Text("%s", skill.name.c_str());
					ImGui::PopStyleColor();

					// レベル表示（プログレスバー）
					float progress = (float)skill.currentLv / (float)skill.maxLv;
					std::string barLabel = std::to_string(skill.currentLv) + "/" + std::to_string(skill.maxLv);
					ImGui::ProgressBar(progress, ImVec2(120, 15), barLabel.c_str());

					// ボタンまたは状態テキスト
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
					ImGui::EndGroup(); // スキルノードここまで
				}

				ImGui::EndChild(); // TreeCanvasここまで
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End(); // メインの「スキルツリー 開発画面」ウィンドウここまで
}