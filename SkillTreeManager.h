#pragma once
#include "SkillCategory.h"
#include <vector>

class SkillTreeManager {
private:
	std::vector<SkillCategory> categories; // すべてのカテゴリー（ステータス、体技など）
	int currentCategoryIndex;              // 現在表示しているカテゴリーの番号（0:ステータス、1:体技...）
	int playerSP;                          // プレイヤーの所持SP

	// ===================================================
	// 【追加】プレイヤーのステータス実変数
	// ===================================================
	float playerAttackPower; // 攻撃力
	float playerMaxHP;       // 最大HP
	float playerMoveSpeed;   // 移動速度

	// ===================================================
	// 【追加】内部処理用のプライベート関数
	// ===================================================
	void ApplySkillEffect(int skillId);

public:
	SkillTreeManager();

	void Initialize();                                            // スキルデータの初期化
	void Update(float mouseX, float mouseY, bool isMouseClicked); // 更新処理
	void Draw();                                                  // 描画処理

	// 便利なゲッター
	int GetPlayerSP() const { return playerSP; }
	int GetCurrentCategoryIndex() const { return currentCategoryIndex; }
	std::vector<SkillCategory>& GetCategories() { return categories; }
};