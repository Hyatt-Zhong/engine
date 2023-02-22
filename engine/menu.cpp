#include "menu.h"

using namespace ns_engine;
namespace ns_menu {
void Menu::OnMouseMove(const int &x, const int &y) {
	auto xx = x;
	auto yy = y;
	camera_->Trans(xx, yy);
	if (InRange(xx, yy)) {
	} //更新in_range_标志，将自动调用OnHover
	Layer::OnMouseMove(x, y);
}
void Menu::OnHover(const int &x, const int &y, const int &w, const int &h) {
	Game::Instance()->DrawRect(x, y, w, h, 0x0000ffff);
}
void Button::OnLMouseUp(const int &x, const int &y) {
	auto xx = x;
	auto yy = y;
	camera_->Trans(xx, yy);
	if (InRange(xx, yy)) {
		OnClick();
	}
}
void Button::OnMouseMove(const int &x, const int &y) {
	auto xx = x;
	auto yy = y;
	camera_->Trans(xx, yy);
	if (InRange(xx, yy)) {
	} //更新in_range_标志，将自动调用OnHover
}
void Button::OnHover(const int &x, const int &y, const int &w, const int &h) {
	Game::Instance()->DrawRect(x, y, w, h, 0x0000ffff);
}

void Blood::Change(const BloodData &bd) {
	auto val = bd.val * base_width_;
	auto max = bd.max * base_width_;
	if (val <= 0) {
		/*SetAlive(false);
		return;*/
	}
	w_ = val;

	auto x = x_, y = y_;
	camera_->Trans(x, y, h_);
	if (!center) {
		Game::Instance()->DrawRect(x, y, max, h_, 0xff);
	} else {
		x = (Game::Instance()->w_ - max) / 2;
		Game::Instance()->DrawRect(x, y, max, h_, 0xff);
		x_ = x;
	}
}
void Blood::OnNotice(void *data) {
	SetAlive(true);
	auto pval = (BloodData *)data;
	Change(*pval);
}
void SkillInfo::OnNotice(void *data) {
	auto dt = *(float *)data;
	auto w = w_ - dt * w_;
	int x = x_ + w_ - w;
	w += 3;
	auto y = y_;
	camera_->Trans(x, y, h_);
	Game::Instance()->OverDraw([=]() { Game::Instance()->FillRect(x, y, w, h_, 0x00); });
}
SkillMenu::SkillMenu(const string &name) : Menu(name) {
	auto h = size;
	x_ = 0;
	y_ = Game::Instance()->h_ * 0.95 - h;
	w_ = Game::Instance()->w_;
	h_ = h;
}
void SkillMenu::PushSkill(const string &skill, const string &ico) {
	auto skillico = new SkillInfo;
	skillico->SetAlive(true);
	skillico->SetName(skill);
	AddSub(skillico);
	auto n = sub_.size();
	skillico->SetPostion(x_ + n * (size + dt), y_);
	skillico->SetSize(size, size);
	skillico->AddAssetAnimation(ico, 0, 0);

	skills_[skill] = skillico;
}
void SkillMenu::RemoveSkill(const string &skill) {
	auto it = skills_.find(skill);
	if (it == skills_.end()) {
		return;
	}
	DeleteSub(it->second);
	delete it->second;
}

};
