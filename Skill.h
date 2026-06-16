#pragma once
#include <string>

// ===================================================
// スキルの現在の進行・解放状態を管理する列挙型
// ===================================================
enum class SkillState {
	Locked,    // 前提スキルが未習得のためロックされている状態
	Available, // 解放条件を満たしておりSPがあれば即時習得できる状態
	Unlocked   // レベルが最大まで達してマスターしている状態
};

class Skill {
public:
	// ===================================================
	// メンバ変数
	// ===================================================
	int id;                  // スキルごとに割り振られた固有の識別番号
	std::string name;        // 画面に表示されるスキルの名称
	std::string description; // スキルに関する詳細な説明文

	int currentLv;  // プレイヤーが現在習得している現在のレベル
	int maxLv;      // このスキルが到達できる上限の最大レベル
	int requiredSP; // このスキルのレベルを1上げるために消費するSP量

	SkillState state; // 列挙型による現在のスキルの解放・習得ステータス

	float x;      // 描画キャンバス内におけるノードのX座標
	float y;      // 描画キャンバス内におけるノードのY座標
	float radius; // マウスカーソルとの当たり判定に使用する円の判定半径

	int parentSkillId; // このスキルを解放するために事前に習得が必要な親スキルのID

public:
	// ===================================================
	// コンストラクタ（スキルの初期データを設定）
	// ===================================================
	Skill(int id, std::string name, std::string desc, int maxLv, int sp, float x, float y, int parentId = -1)
	    : id(id), name(name), description(desc), currentLv(0), maxLv(maxLv), requiredSP(sp), state(SkillState::Locked), x(x), y(y), radius(35.0f), parentSkillId(parentId) {

		// 親スキルIDが-1（前提なし）の場合は最初から習得可能な状態にする
		if (parentSkillId == -1) {
			state = SkillState::Available;
		}
	}

	// ===================================================
	// メンバ関数
	// ===================================================

	// ---------------------------------------------------
	// マウスカーソルがノードの当たり判定内にあるかをチェック
	// ---------------------------------------------------
	bool IsMouseOver(float mouseX, float mouseY) {
		float dx = mouseX - x;
		float dy = mouseY - y;
		// 円の判定公式（距離の2乗比較）を用いて内側にあるかを判定
		return (dx * dx + dy * dy) <= (radius * radius);
	}

	// ---------------------------------------------------
	// スキルのレベルを1加算するレベルアップ処理
	// ---------------------------------------------------
	void LevelUp() {
		if (currentLv < maxLv) {
			currentLv++;
			// レベルが最大値に達したらマスター（Unlocked）状態へ移行
			if (currentLv == maxLv) {
				state = SkillState::Unlocked;
			}
		}
	}
};