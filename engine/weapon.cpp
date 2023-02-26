#include "weapon.h"

#include "entity.h"
#include "map.h"

using namespace ns_engine;

using namespace ns_entity;
using namespace ns_ai;
namespace ns_weapon {
Alpha::Alpha() {}
Alpha::Alpha(const string &bullet) : bullet_(bullet), direct_(0.f, 1.f) {}

void Weapon::SetMaster(Actor *master) {
	master_ = master;
	layer_ = master_->parent_;
}

void Alpha::Use() {
	frame();
	if (frame(30) && master_->parent_) {
		auto layer = master_->parent_;
		int x, y;
		dynamic_cast<ModuleInstance *>(master_)->GetSubGeneratePos(x, y);
		ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>(bullet_, (Layer *)layer, master_, x, y, "");
	}
}

void Alpha::SetBullet(const string &bullet) {
	bullet_ = bullet;
}



Weapon *WPAlpha() {
	auto wp = new Alpha("bullet");
	return wp;
}
Weapon *WPSlowAlpha() {
	return new Alpha("slowbullet");
}
Weapon *WPFollowBullet() {
	return new Alpha("FollowBullet");
}
ShotOnce::ShotOnce(const string &bullet) {
	bullet_ = bullet;
}
void ShotOnce::Use() {
	if (master_->parent_) {
		auto layer = master_->parent_;
		int x, y;
		dynamic_cast<ModuleInstance *>(master_)->GetSubGeneratePos(x, y);
		ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>(bullet_, (Layer *)layer, master_, x, y, "");
		dynamic_cast<ModuleInstance *>(master_)->UseWeapon(false);
	}
}

Weapon *WPShotOnce() {
	return new ShotOnce("slowbullet");
}
};
