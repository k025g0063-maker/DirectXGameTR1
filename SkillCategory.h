#pragma once
#include "Skill.h"
#include <string>
#include <vector>

class SkillCategory {
private:
	std::string categoryName;
	std::vector<Skill> skills;

public:
	SkillCategory(std::string name);

	std::string GetName() const;
	void AddSkill(const Skill& skill);
	std::vector<Skill>& GetSkills();

	// 前提条件をチェックしてロックを解除する関数
	void UpdateSkillStates();
};