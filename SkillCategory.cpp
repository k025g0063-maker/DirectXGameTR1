#pragma once
#include "SkillCategory.h"

// コンストラクタ
SkillCategory::SkillCategory(std::string name) : categoryName(name) {}

// カテゴリー名を取得
std::string SkillCategory::GetName() const { return categoryName; }

// スキルをカテゴリーに追加
void SkillCategory::AddSkill(const Skill& skill) { skills.push_back(skill); }

// スキルのリストを取得
std::vector<Skill>& SkillCategory::GetSkills() { return skills; }

// ロック解除の更新処理
void SkillCategory::UpdateSkillStates() {
	for (auto& skill : skills) {
		if (skill.state != SkillState::Locked)
			continue;

		if (skill.parentSkillId != -1) {
			for (const auto& parent : skills) {
				if (parent.id == skill.parentSkillId) {
					// 前提スキルが習得済み（レベルが1以上、または最大レベル）なら解放可能に
					if (parent.currentLv > 0) {
						skill.state = SkillState::Available;
					}
					break;
				}
			}
		}
	}
}