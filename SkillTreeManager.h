#pragma once
#include "SkillCategory.h"
#include <vector>

class SkillTreeManager {
private:
	// ===================================================
	// メンバ変数
	// ===================================================
	std::vector<SkillCategory> categories; // すべてのスキルカテゴリーを保持する配列
	int currentCategoryIndex;              // 現在画面に表示しているタブカテゴリーの配列番号
	int playerSP;                          // プレイヤーが現在所有しているスキルポイント

	float playerAttackPower; // プレイヤーの現在の攻撃力
	float playerMaxHP;       // プレイヤーの現在の最大HP
	float playerMoveSpeed;   // プレイヤーの現在の移動速度

	// ===================================================
	// 内部処理用プライベート関数
	// ===================================================
	void ApplySkillEffect(int skillId); // スキルの効果をプレイヤーパラメータに加算する処理
	void ResetAllSkills();              // すべてのスキルとステータスを初期状態に巻き戻す処理

public:
	// ===================================================
	// メンバ関数
	// ===================================================
	SkillTreeManager();

	void Initialize();                                            // 全てのスキル構造データを構築・初期化する関数
	void Update(float mouseX, float mouseY, bool isMouseClicked); // マウスクリック等によるシステム状態の更新関数
	void Draw();                                                  // ImGuiを用いたグラフィカルな画面描画関数

	// 外部から各データを安全に取得するためのゲッター関数群
	int GetPlayerSP() const { return playerSP; }
	int GetCurrentCategoryIndex() const { return currentCategoryIndex; }
	std::vector<SkillCategory>& GetCategories() { return categories; }
};