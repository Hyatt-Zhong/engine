#include "physic-module.h"

using namespace ns_engine;

namespace ns_physic_module {
	using namespace ns_box2d;

b2Body *CreateBodyFunc(Actor *actor,         //纹理对象
		       b2BodyType type,      //类型
		       bool isSensor,        //参与碰撞
		       bool isFixedRotation, //不让旋转
		       float mass,           //质量
		       float restitution,    //反弹
		       float linearDamping,  //线性阻尼
		       float friction        //摩擦
) {
	auto info = actor->GetInfo();
	info.div(nx);

	auto [x, y, w, h] = info;
	//////////////////////
	b2BodyDef bodydef;
	bodydef.userData.pointer = (uintptr_t)actor;
	//默认静态
	//bodydef.type = b2_staticBody;
	bodydef.type = type /*: b2_staticBody*/;
	//刚体位置就是精灵位置
	bodydef.position.Set(x + w / 2, y + h / 2);
	//线性阻尼
	bodydef.linearDamping = linearDamping;
	auto body = actor->world_->World()->CreateBody(&bodydef);

	b2FixtureDef fixtureDef;
	//物体的密度
	//fixtureDef.density = .1;
	//物体的摩擦
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;

	fixtureDef.filter.categoryBits = kCommon;
	//对象之间有碰撞检测但是又不想让它们有碰撞反应，那么你就需要把isSensor设置成true
	fixtureDef.isSensor = isSensor;

	b2PolygonShape bodyShape;

	bodyShape.SetAsBox(w / 2, h / 2);
	fixtureDef.shape = &bodyShape;
	body->CreateFixture(&fixtureDef);

	body->SetFixedRotation(isFixedRotation);

	b2MassData md;
	body->GetMassData(&md);
	md.mass = mass;
	body->SetMassData(&md);

	return body;
}

b2Body *NoCollFunc(Actor *actor) {
	return CreateBodyFunc(actor, b2_dynamicBody, true, true, 1, 0, 0, 0);
}
b2Body *CollFunc(Actor *actor) {
	return CreateBodyFunc(actor, b2_dynamicBody, false, true, .0001, 1, 0, 0);
}
b2Body *CollNoRestiFunc(Actor *actor) {
	return CreateBodyFunc(actor, b2_dynamicBody, false, true, 1, 0, 0, 0);
}
};
