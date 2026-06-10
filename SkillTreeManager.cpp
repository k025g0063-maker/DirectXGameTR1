#include "SkillTreeManager.h"

// ★メインファイルと同じように、インクルードパスの「externals/」を外しました
#include "imgui.h"

// コンストラクタ
SkillTreeManager::SkillTreeManager() : currentCategoryIndex(0), playerSP(10) {}

// スキルツリーのデータ初期化
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

// 更新処理
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
					currentCategory.UpdateSkillStates();
				}
			}
		}
	}
}

// 描画処理
void SkillTreeManager::Draw() {
	// 1. スキルツリー用のウィンドウを作成
	ImGui::Begin("スキルツリー画面");

	// プレイヤーの所持SPを表示
	ImGui::Text("所持SP: %d", playerSP);
	ImGui::Separator();

	// 2. カテゴリー（タブ）の切り替え部分
	if (ImGui::BeginTabBar("CategoryTabBar")) {
		for (int i = 0; i < categories.size(); ++i) {
			// タブを生成
			if (ImGui::BeginTabItem(categories[i].GetName().c_str())) {
				currentCategoryIndex = i;

				ImGui::Spacing();
				ImGui::Text("--- %s ツリー ---", categories[i].GetName().c_str());
				ImGui::Spacing();

				// 3. タブの中身（スキル一覧）を表示
				for (auto& skill : categories[i].GetSkills()) {

					// スキルの状態によって文字の色を変える
					if (skill.state == SkillState::Locked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 灰色
					} else if (skill.state == SkillState::Available) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // 緑色
					}

					// スキルの情報をテキスト表示
					ImGui::Text("[%s] Lv:%d/%d (必要SP:%d)", skill.name.c_str(), skill.currentLv, skill.maxLv, skill.requiredSP);

					ImGui::PopStyleColor(); // 色を元に戻す

					ImGui::SameLine();

					// ボタンの作成
					if (skill.state == SkillState::Locked) {
						ImGui::Text(" (ロック中) ");
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::Text(" (マスター!) ");
					} else {
						// 解放可能状態なら、ボタンを設置
						std::string buttonLabel = "習得する##" + std::to_string(skill.id);
						if (ImGui::Button(buttonLabel.c_str())) {
							if (playerSP >= skill.requiredSP) {
								playerSP -= skill.requiredSP;
								skill.LevelUp();
								categories[i].UpdateSkillStates();
							}
						}
					}

					// スキルの説明文
					ImGui::TextDisabled("   説明: %s", skill.description.c_str());
					ImGui::Separator();
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}