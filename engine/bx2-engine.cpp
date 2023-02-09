#include "bx2-engine.h"
#include "engine.h"

using namespace ns_engine;
namespace ns_box2d {

const int nx = 32;//绘制坐标和物理坐标的转换倍数

bx2DbgDraw::bx2DbgDraw(ns_engine::Camera *xs)
{
	x = xs;
}

bx2DbgDraw::~bx2DbgDraw() {}
void bx2DbgDraw::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount,
			   const b2Color &color)
{
	auto r = x->win_->render_;
	Uint8 red, g, b, a;
	SDL_GetRenderDrawColor(r, &red, &g, &b, &a);
	Uint8 nr = 255 * color.r;
	Uint8 ng = 255 * color.g;
	Uint8 nb = 255 * color.b;
	Uint8 na = 255 * color.a;

	SDL_SetRenderDrawColor(r, nr, ng, nb, na);

	SDL_Point *sp = new SDL_Point[vertexCount];
	b2point2sdlpoint(sp, vertices, vertexCount);
	SDL_RenderDrawLines(r, sp, vertexCount);
	SAFE_DELETE_ARRAY(sp);

	SDL_SetRenderDrawColor(r, red, g, b, a);
}
void bx2DbgDraw::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount,
				const b2Color &color)
{
	auto r = x->win_->render_;
	Uint8 red, g, b, a;
	SDL_GetRenderDrawColor(r, &red, &g, &b, &a);
	Uint8 nr = 255 * color.r;
	Uint8 ng = 255 * color.g;
	Uint8 nb = 255 * color.b;
	Uint8 na = 255 * color.a;

	SDL_SetRenderDrawColor(r, nr, ng, nb, na);

	SDL_Point *sp = new SDL_Point[vertexCount];
	b2point2sdlpoint(sp, vertices, vertexCount);
	SDL_RenderDrawLines(r, sp, vertexCount);
	auto p1 = sp[0];
	auto pe = sp[vertexCount - 1];
	SDL_RenderDrawLine(r, p1.x, p1.y, pe.x, pe.y);
	SAFE_DELETE_ARRAY(sp);

	SDL_SetRenderDrawColor(r, red, g, b, a);
}
void bx2DbgDraw::DrawCircle(const b2Vec2 &center, float radius,
			  const b2Color &color)
{
}
void bx2DbgDraw::DrawSolidCircle(const b2Vec2 &center, float radius,
			       const b2Vec2 &axis, const b2Color &color)
{
}
void bx2DbgDraw::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2,
			   const b2Color &color)
{
	auto r = x->win_->render_;

	Uint8 red, g, b, a;
	SDL_GetRenderDrawColor(r, &red, &g, &b, &a);
	Uint8 nr = 255 * color.r;
	Uint8 ng = 255 * color.g;
	Uint8 nb = 255 * color.b;
	Uint8 na = 255 * color.a;

	SDL_SetRenderDrawColor(r, nr, ng, nb, na);

	auto sp1 = b2point2sdlpoint(p1);
	auto sp2 = b2point2sdlpoint(p2);
	SDL_RenderDrawLine(r, sp1.x, sp1.y, sp2.x, sp2.y);

	SDL_SetRenderDrawColor(r, red, g, b, a);
}
void bx2DbgDraw::DrawTransform(const b2Transform &xf) {}
void bx2DbgDraw::DrawPoint(const b2Vec2 &p, float size, const b2Color &color) {}
void bx2DbgDraw::DrawString(int x, int y, const char *string, ...) {}
void bx2DbgDraw::DrawString(const b2Vec2 &p, const char *string, ...) {}
void bx2DbgDraw::DrawAABB(b2AABB *aabb, const b2Color &color) {}
void bx2DbgDraw::Flush() {}
SDL_Point bx2DbgDraw::b2point2sdlpoint(const b2Vec2 &bp)
{
	SDL_Point ret;
	auto &[x, y] = ret;
	auto m = bp.x * nx;
	auto n = bp.y * nx;
	dbg_rect obj = {m, n, 0, 0};
	bx2DbgDraw::x->Catch(&obj, x, y);
	return ret;
}
void bx2DbgDraw::b2point2sdlpoint(SDL_Point *sp, const b2Vec2 *bp, int count)
{
	for (int i = 0; i < count; i++) {
		auto &[x, y] = sp[i];
		auto m = bp[i].x * nx;
		auto n = bp[i].y * nx;
		dbg_rect obj = {m, n, 0, 0};
		bx2DbgDraw::x->Catch(&obj, x, y);
	}
}

b2Body *bx2World::CreateBody(Actor *actor) {
	if (actor->create_body_) {
		return actor->create_body_(actor);
	}

	auto info = actor->GetInfo();
	info.div(nx);
	auto [x, y, w, h] = info;

	b2BodyDef bodyInfo;
	bodyInfo.position.Set(x + w / 2, y + h / 2);

	auto body = world_->CreateBody(&bodyInfo);

	b2PolygonShape shape;
	shape.SetAsBox(w / 2, h / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.filter.categoryBits = kBuilding;
	body->CreateFixture(&fixture);

	return body;
}

void bx2World::UpdateElement() {
	if (box2d_drive_)	{
		for (auto &it : relate_) {
			auto [x, y] = it.second->GetPosition();
			x *= nx;
			y *= nx;
			it.first->SetPostion(x, y);
		}
	} else {
		for (auto &it : relate_) {
			float x = it.first->x_ + it.first->w_ / 2;
			float y = it.first->y_ + it.first->h_ / 2;

			x /= nx;
			y /= nx;
			it.second->SetTransform(
				b2Vec2(x,y), 0);
		}
	}
}

b2Body *bx2World::CreateStaticBody(const int &xx, const int &yy, const int &ww,
				   const int &hh) {
	SizeInfo info(xx, yy, ww, hh);
	info.div(nx);

	auto [x, y, w, h] = info;
	
	b2BodyDef bodyInfo;
	bodyInfo.position.Set(x + w / 2, y + h / 2);

	auto body = world_->CreateBody(&bodyInfo);

	b2PolygonShape shape;
	shape.SetAsBox(w / 2, h / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.filter.categoryBits = kBuilding;
	body->CreateFixture(&fixture);

	return body;
}

b2Body *SampleFunc(Actor *actor) {
	auto info = actor->GetInfo();
	info.div(nx);

	auto [x, y, w, h] = info;
	//////////////////////
	b2BodyDef bodydef;
	bodydef.userData.pointer = (uintptr_t)actor;
	//默认静态
	//bodydef.type = b2_staticBody;
	bodydef.type = b2_dynamicBody /*: b2_staticBody*/;
	//刚体位置就是精灵位置
	bodydef.position.Set(x + w / 2, y + h / 2);
	//线性阻尼
	//bodydef.linearDamping = 0.6f;
	auto body = actor->world_->World()->CreateBody(&bodydef);

	b2FixtureDef fixtureDef;
	//物体的密度
	//fixtureDef.density = .1;
	//物体的摩擦
	fixtureDef.friction = 0.0;
	fixtureDef.restitution = 0;

	fixtureDef.filter.categoryBits = kCommon;
	//对象之间有碰撞检测但是又不想让它们有碰撞反应，那么你就需要把isSensor设置成true
	//fixtureDef.isSensor = true;

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
