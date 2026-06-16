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
	// 既存のデータをクリアして初期状態に戻す
	categories.clear();

	// ---------------------------------------------------
	// ステータスカテゴリーの作成とスキル登録
	// ---------------------------------------------------
	SkillCategory statusCategory("ステータス");

	// 腕立て伏せルート（左側ツリーのノード登録）
	statusCategory.AddSkill(Skill(1, "腕立て伏せ", "基礎的な筋力を鍛えます。", 1, 1, 300.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(2, "物理攻撃アップ", "攻撃力が上昇します。", 5, 2, 200.0f, 400.0f, 1));
	statusCategory.AddSkill(Skill(3, "最大HPアップ(腕)", "最大HPが上昇します。", 5, 2, 400.0f, 400.0f, 1));

	// スクワットルート（右側ツリーのノード登録）
	statusCategory.AddSkill(Skill(4, "スクワット", "下半身の基礎体力を鍛えます。", 1, 1, 800.0f, 200.0f, -1));
	statusCategory.AddSkill(Skill(5, "素早さアップ", "移動速度が上昇します。", 5, 2, 700.0f, 400.0f, 4));
	statusCategory.AddSkill(Skill(6, "最大HPアップ(脚)", "最大HPが上昇します。", 5, 2, 900.0f, 400.0f, 4));

	// 管理用配列にカテゴリーを追加
	categories.push_back(statusCategory);

	// ---------------------------------------------------
	// 体技カテゴリーの作成とスキル登録
	// ---------------------------------------------------
	SkillCategory taigiCategory("体技");
	taigiCategory.AddSkill(Skill(101, "体当たり", "敵に突進してダメージを与えます。", 1, 2, 550.0f, 200.0f, -1));
	categories.push_back(taigiCategory);

	// ---------------------------------------------------
	// 呪文・探索カテゴリーの空枠作成
	// ---------------------------------------------------
	SkillCategory jmonCategory("呪文");
	categories.push_back(jmonCategory);

	SkillCategory tansakuCategory("探索");
	categories.push_back(tansakuCategory);
}

// ===================================================
// スキル習得時のステータス上昇処理
// ===================================================
void SkillTreeManager::ApplySkillEffect(int skillId) {
	// スキルIDに応じてプレイヤーのパラメータを加算
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
	default:
		break;
	}
}

// ===================================================
// スキルリセットおよびSP返還処理
// ===================================================
void SkillTreeManager::ResetAllSkills() {
	// プレイヤーの各種パラメータを初期値に巻き戻す
	playerAttackPower = 10.0f;
	playerMaxHP = 100.0f;
	playerMoveSpeed = 2.0f;

	// 全カテゴリーの全スキルを走査してリセット
	for (auto& category : categories) {
		for (auto& skill : category.GetSkills()) {
			// 習得済みのスキルから消費SPを計算して返還
			if (skill.currentLv > 0) {
				playerSP += (skill.requiredSP * skill.currentLv);
				skill.currentLv = 0;
			}

			// 前提スキルの有無に応じて初期の解放状態を再設定
			if (skill.parentSkillId == -1) {
				skill.state = SkillState::Available;
			} else {
				skill.state = SkillState::Locked;
			}
		}
		// カテゴリー全体のロック状態の整合性を再計算
		category.UpdateSkillStates();
	}
}

// ===================================================
// 更新処理
// ===================================================
void SkillTreeManager::Update(float mouseX, float mouseY, bool isMouseClicked) {
	if (categories.empty())
		return;

	// 現在アクティブなカテゴリーを取得
	auto& currentCategory = categories[currentCategoryIndex];

	// マウスクリック時の当たり判定と習得処理
	if (isMouseClicked) {
		for (auto& skill : currentCategory.GetSkills()) {
			// マウスがノード内にあり、かつ習得可能な状態かを判定
			if (skill.IsMouseOver(mouseX, mouseY) && skill.state == SkillState::Available) {
				// 所持SPが足りていれば消費してレベルアップを実行
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
	// メインの開発者用ウィンドウを生成
	ImGui::Begin("スキルツリー 開発画面", nullptr, ImGuiWindowFlags_NoScrollbar);

	// ---------------------------------------------------
	// ステータス表示エリア
	// ---------------------------------------------------
	// ウィンドウ上部にプレイヤー情報を表示する独立フレームを作成
	ImGui::BeginChild("StatusFrame", ImVec2(0, 75), true);

	ImGui::Text("【 プレイヤー現在のステータス 】");
	ImGui::Text("所持SP: %d", playerSP);
	ImGui::SameLine();
	ImGui::Text(" | 攻撃力: %.1f", playerAttackPower);
	ImGui::SameLine();
	ImGui::Text(" | 最大HP: %.1f", playerMaxHP);
	ImGui::SameLine();
	ImGui::Text(" | 移動速度: %.1f", playerMoveSpeed);

	// 右端にデバッグ用のボタン群を並べて配置
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 340);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

	// 全リセット処理を呼び出すボタン
	if (ImGui::Button("スキル全リセット", ImVec2(150, 25))) {
		ResetAllSkills();
	}

	ImGui::SameLine();

	// テスト用にSPを追加獲得するボタン
	if (ImGui::Button("レベルアップ (SP+3)", ImVec2(150, 25))) {
		playerSP += 3;
	}

	ImGui::EndChild();
	ImGui::Spacing();

	// ---------------------------------------------------
	// カテゴリー切り替えタブエリア
	// ---------------------------------------------------
	// カテゴリーごとに画面を切り替えるタブバーを開始
	if (ImGui::BeginTabBar("CategoryTabBar")) {
		for (int i = 0; i < categories.size(); ++i) {
			if (ImGui::BeginTabItem(categories[i].GetName().c_str())) {
				// 選択されたタブのインデックスを保存
				currentCategoryIndex = i;

				ImGui::Spacing();
				ImGui::Text("--- %s ツリー ---", categories[i].GetName().c_str());
				ImGui::Spacing();

				// スキルを自由配置するためのスクロール可能な子キャンバスを開始
				ImGui::BeginChild("TreeCanvas", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

				// 背景に直線を引くための描画リストと基準となる左上絶対座標を取得
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 basePos = ImGui::GetCursorScreenPos();

				// ステータスタブが表示されている時のみツリーの接続線を描画
				if (categories[i].GetName() == "ステータス") {
					ImU32 lineColor = IM_COL32(255, 255, 255, 255);
					float lineThickness = 2.0f;

					// 各ノードを綺麗に結ぶための接続ポイントをドット単位で計算
					ImVec2 pos1 = ImVec2(basePos.x + 150.0f + 60.0f, basePos.y + 40.0f + 70.0f); // 腕立て伏せボタンの下端
					ImVec2 pos2 = ImVec2(basePos.x + 50.0f + 60.0f, basePos.y + 215.0f);         // 物理攻撃テキストの上端
					ImVec2 pos3 = ImVec2(basePos.x + 250.0f + 60.0f, basePos.y + 215.0f);        // 最大HP(腕)テキストの上端

					ImVec2 pos4 = ImVec2(basePos.x + 650.0f + 60.0f, basePos.y + 40.0f + 70.0f); // スクワットボタンの下端
					ImVec2 pos5 = ImVec2(basePos.x + 550.0f + 60.0f, basePos.y + 215.0f);        // 素早さテキストの上端
					ImVec2 pos6 = ImVec2(basePos.x + 750.0f + 60.0f, basePos.y + 215.0f);        // 最大HP(脚)テキストの上端

					// 計算した接続ポイント間に背景線を引く
					drawList->AddLine(pos1, pos2, lineColor, lineThickness);
					drawList->AddLine(pos1, pos3, lineColor, lineThickness);
					drawList->AddLine(pos4, pos5, lineColor, lineThickness);
					drawList->AddLine(pos4, pos6, lineColor, lineThickness);
				}

				// ---------------------------------------------------
				// スキルノード群の個別描画
				// ---------------------------------------------------
				for (auto& skill : categories[i].GetSkills()) {

					float posX = 0.0f;
					float posY = 0.0f;

					// スキルIDごとに設定された独自の2Dグリッド座標を割り当て
					switch (skill.id) {
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
					case 101:
						posX = 400.0f;
						posY = 130.0f;
						break;
					default:
						posX = 50.0f;
						posY = 50.0f;
						break;
					}

					// カーソル位置を指定座標へ移動させ、ノードの描画グループを開始
					ImGui::SetCursorPos(ImVec2(posX, posY));
					ImGui::BeginGroup();

					// ロック・習得可能・マスターの状態に応じてテキストの色を変更
					if (skill.state == SkillState::Locked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 灰色
					} else if (skill.state == SkillState::Available) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // 緑色
					}

					// スキル名の表示
					ImGui::Text("%s", skill.name.c_str());
					ImGui::PopStyleColor();

					// 現在のレベルを視覚的に表すプログレスバーを表示
					float progress = (float)skill.currentLv / (float)skill.maxLv;
					std::string barLabel = std::to_string(skill.currentLv) + "/" + std::to_string(skill.maxLv);
					ImGui::ProgressBar(progress, ImVec2(120, 15), barLabel.c_str());

					// スキル状態に基づいたインタラクション要素（ボタン・ラベル）の描画
					if (skill.state == SkillState::Locked) {
						ImGui::TextDisabled("[ロック中]");
					} else if (skill.state == SkillState::Unlocked) {
						ImGui::Text("[マスター]");
					} else {
						// 習得ボタンの描画と、クリック時のSP消費およびレベルアップ処理
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