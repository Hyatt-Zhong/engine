#include "physic-module.h"

using namespace ns_engine;

namespace ns_physic_module {
	using namespace ns_box2d;

b2Body *SampleFunc(Actor *actor) {
	auto info = actor->GetInfo();
	info.div(nx);

	auto [x, y, w, h] = info;
	//////////////////////
	b2BodyDef bodydef;
	bodydef.userData.pointer = (uintptr_t)actor;
	//Ĭ�Ͼ�̬
	//bodydef.type = b2_staticBody;
	bodydef.type = b2_dynamicBody /*: b2_staticBody*/;
	//����λ�þ��Ǿ���λ��
	bodydef.position.Set(x + w / 2, y + h / 2);
	//��������
	//bodydef.linearDamping = 0.6f;
	auto body = actor->world_->World()->CreateBody(&bodydef);

	b2FixtureDef fixtureDef;
	//������ܶ�
	//fixtureDef.density = .1;
	//�����Ħ��
	fixtureDef.friction = 0.0;
	fixtureDef.restitution = 0;

	fixtureDef.filter.categoryBits = kCommon;
	//����֮������ײ��⵫���ֲ�������������ײ��Ӧ����ô�����Ҫ��isSensor���ó�true
	fixtureDef.isSensor = true;

	b2PolygonShape bodyShape;

	bodyShape.SetAsBox(w / 2, h / 2);
	fixtureDef.shape = &bodyShape;
	body->CreateFixture(&fixtureDef);

	body->SetFixedRotation(true);

	b2MassData md;
	body->GetMassData(&md);
	md.mass = 1;
	body->SetMassData(&md);

	return body;
}

};
