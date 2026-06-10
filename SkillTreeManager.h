#pragma once
#include "SkillCategory.h"
#include <vector>

class SkillTreeManager {
private:
	std::vector<SkillCategory> categories; // すべてのカテゴリー（ステータス、体技など）
	int currentCategoryIndex;              // 現在表示しているカテゴリーの番号（0:ステータス、1:体技...）
	int playerSP;                          // プレイヤーの所持SP

public:
	SkillTreeManager();

	void Initialize();                                            // スキルデータの初期化（腕立て伏せなどを登録する）
	void Update(float mouseX, float mouseY, bool isMouseClicked); // 更新処理
	void Draw();                                                  // 描画処理

	// 便利なゲッター
	int GetPlayerSP() const { return playerSP; }
	int GetCurrentCategoryIndex() const { return currentCategoryIndex; }
	std::vector<SkillCategory>& GetCategories() { return categories; }
};