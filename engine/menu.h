#ifndef MENU_H
#define MENU_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_menu {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

class Menu : public Layer {
public:
	using Layer::Layer;
	void OnMouseMove(const int& x, const int& y) {
		auto xx = x;
		auto yy = y;
		camera_->Trans(xx, yy);
		if (InRange(xx, yy)) {}//更新in_range_标志，将自动调用OnHover
		Layer::OnMouseMove(x, y);
	}

	void OnHover(const int &x, const int &y, const int &w, const int &h) { 
		Game::Instance()->DrawRect(x, y, w, h, 0x0000ffff);
	}
protected:
private:
};

class Button : public Actor {
public:
	void OnLMouseUp(const int &x, const int &y) {
		auto xx = x;
		auto yy = y;
		camera_->Trans(xx, yy);
		if (InRange(xx, yy)) {
			OnClick();
		}
	}

	void OnMouseMove(const int& x, const int& y) {
		auto xx = x;
		auto yy = y;
		camera_->Trans(xx, yy);
		if (InRange(xx, yy)) {}//更新in_range_标志，将自动调用OnHover
	}

	void OnHover(const int &x, const int &y, const int &w, const int &h) { 
		Game::Instance()->DrawRect(x, y, w, h, 0x0000ffff);
	}
	virtual void OnClick() = 0;

protected:
private:
};

class EventButton:public Button {
public:
	void OnClick() {}

protected:
private:
};
class Link:public Button
{
public:
	enum LinkType {
		kScene,
		kLayer
	};
	Link(const string &goal, LinkType type) : goal_(goal), type_(type) {}
	Link(const string &goal, bool show) : goal_(goal), type_(kLayer), show_(show) {}
	void LinkScene(const string &scene) { Game::Instance()->SwitchScene(scene);	}
	void LinkLayer(const string &layer) { Game::Instance()->ShowLayer(layer, show_); }
	void OnClick() {
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

protected:
private:
	string goal_;
	LinkType type_;
	bool show_;
};
class Info;
using notice_func = function<void(void *, Info*)>;
class Info : public Actor {
public:
	Info(const string &name) { SetName(name); }
	Info(const string &name, const notice_func &func) : func_(func) { SetName(name); }
	void OnNotice(void *data) { func_(data,this);}

protected:
private:
	notice_func func_;
};

class Blood:public Info
{
public:
	using Info::Info;//继承构造函数
	void Init(float max) {
		val_ = max;
		max_ = max;
		max_length_ = w_;
	}

	void Change(int val) {
		val_ += val;
		auto x = val_ <= 0 ? 0.f : val_ / max_;
		w_ = x >= 1 ? max_length_ : max_length_ * x;
	}
	void OnNotice(void *data) {
		auto pval = (int *)data;
		Change(*pval);
		delete pval;
	}

protected:
private:
	float val_;
	float max_;
	float max_length_;
};
};
#endif
