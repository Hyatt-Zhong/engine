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
	//using Layer::Layer;
	Menu(const string &name) : Layer(name) {
		//不需要自动清理，比如菜单层
		auto_clear_sub_ = false;
	}
	void OnMouseMove(const int &x, const int &y);

	void OnHover(const int &x, const int &y, const int &w, const int &h);

protected:
private:
};

class Button : public Actor {
public:
	void OnLMouseUp(const int &x, const int &y);

	void OnMouseMove(const int &x, const int &y);

	void OnHover(const int &x, const int &y, const int &w, const int &h);
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
	void LinkScene(const string &scene);
	void LinkLayer(const string &layer);
	void OnClick();

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
	struct BloodData {
		float val;
		float max;
	};
	//using Info::Info;//继承构造函数
	Blood(const string &name) : Info(name) { SetAlive(false); }
	void Center() { center = true; }
	void Change(const BloodData &bd);
	void OnNotice(void *data);

protected:
private:
	float base_width_ = .2f; //每滴血对应的长度
	bool center = false;
};

class SkillInfo : public Actor {
public:
	SkillInfo() { SetAlive(false); }
	void OnNotice(void *data);

protected:
private:
};
};
#endif
