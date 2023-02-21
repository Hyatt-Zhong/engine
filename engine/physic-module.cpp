#include "physic-module.h"

using namespace ns_engine;

namespace ns_physic_module {
	using namespace ns_box2d;

b2Body *CreateBodyFunc(Actor *actor,         //�������
		       b2BodyType type,      //����
		       bool isSensor,        //������ײ
		       bool isFixedRotation, //������ת
		       float mass,           //����
		       float restitution,    //����
		       float linearDamping,  //��������
		       float friction        //Ħ��
) {
	auto info = actor->GetInfo();
	info.div(nx);

	auto [x, y, w, h] = info;
	//////////////////////
	b2BodyDef bodydef;
	bodydef.userData.pointer = (uintptr_t)actor;
	//Ĭ�Ͼ�̬
	//bodydef.type = b2_staticBody;
	bodydef.type = type /*: b2_staticBody*/;
	//����λ�þ��Ǿ���λ��
	bodydef.position.Set(x + w / 2, y + h / 2);
	//��������
	bodydef.linearDamping = linearDamping;
	auto body = actor->world_->World()->CreateBody(&bodydef);

	b2FixtureDef fixtureDef;
	//������ܶ�
	//fixtureDef.density = .1;
	//�����Ħ��
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;

	fixtureDef.filter.categoryBits = kCommon;
	//����֮������ײ��⵫���ֲ�������������ײ��Ӧ����ô�����Ҫ��isSensor���ó�true
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
