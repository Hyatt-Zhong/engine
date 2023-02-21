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
void Link::LinkScene(const string &scene) {
	Game::Instance()->SwitchScene(scene);
}
void Link::LinkLayer(const string &layer) {
	Game::Instance()->ShowLayer(layer, show_);
}
void Link::OnClick() {
	switch (type_) {
	case ns_menu::Link::kScene:
		LinkScene(goal_);
		break;
	case ns_menu::Link::kLayer:
		LinkLayer(goal_);
		break;
	default:
		break;
	}
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
	delete pval;
}
};
