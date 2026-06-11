#pragma once
#include <string>

// スキルの現在の状態を表す列挙型
enum class SkillState {
	Locked,    // ロックされている（前提スキルが未習得）
	Available, // 解放可能（前提スキルを習得済み、SPがあれば覚えられる）
	Unlocked   // 習得済み
};

class Skill {
public:
	// --- メンバ変数（データ） ---
	int id;                  // スキルID（固有番号）
	std::string name;        // スキル名
	std::string description; // 説明文

	int currentLv;  // 現在のレベル
	int maxLv;      // 最大レベル
	int requiredSP; // 必要なSP

	SkillState state; // スキルの状態

	// --- 2D用の位置データ ---
	float x;      // 画面のX座標（例: 400）
	float y;      // 画面のY座標（例: 300）
	float radius; // マウス当たり判定用の半径（ピクセル）

	int parentSkillId; // 前提スキルのID（-1は前提なし）

public:
	// コンストラクタ
	Skill(int id, std::string name, std::string desc, int maxLv, int sp, float x, float y, int parentId = -1)
	    : id(id), name(name), description(desc), currentLv(0), maxLv(maxLv), requiredSP(sp), state(SkillState::Locked), x(x), y(y), radius(35.0f), parentSkillId(parentId) {

		// 前提スキルがなければ最初から解放可能
		if (parentSkillId == -1) {
			state = SkillState::Available;
		}
	}

	// --- メンバ関数（処理） ---

	// マウスがスキルの円の中に入っているかを判定する関数
	bool IsMouseOver(float mouseX, float mouseY) {
		float dx = mouseX - x;
		float dy = mouseY - y;
		return (dx * dx + dy * dy) <= (radius * radius);
	}

	// レベルアップ処理
	void LevelUp() {
		if (currentLv < maxLv) {
			currentLv++;
			if (currentLv == maxLv) {
				state = SkillState::Unlocked;
			}
		}
	}
};