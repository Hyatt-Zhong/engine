#ifndef BOX2D_COLLSION_H
#define BOX2D_COLLSION_H

#include <common.h>
#include <box2d/box2d.h>
#include "sdl_windx.h"
using std::map;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;

namespace ns_engine {
class Camera;
class Actor;
};

namespace ns_box2d {

class bx2DbgDraw : public b2Draw {
public:
	bx2DbgDraw(ns_engine::Camera *xs);
	~bx2DbgDraw();
	void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount,
			 const b2Color &color);

	void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount,
			      const b2Color &color);

	void DrawCircle(const b2Vec2 &center, float radius,
			const b2Color &color);

	void DrawSolidCircle(const b2Vec2 &center, float radius,
			     const b2Vec2 &axis, const b2Color &color);

	void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2,
			 const b2Color &color);

	void DrawTransform(const b2Transform &xf);

	void DrawPoint(const b2Vec2 &p, float size, const b2Color &color);

	void DrawString(int x, int y, const char *string, ...);

	void DrawString(const b2Vec2 &p, const char *string, ...);

	void DrawAABB(b2AABB *aabb, const b2Color &color);

	void Flush();
	SDL_Point b2point2sdlpoint(const b2Vec2 &bp);
	void b2point2sdlpoint(SDL_Point *sp, const b2Vec2 *bp, int count);

private:
	struct dbg_rect 
	{
		int x_, y_, w_, h_;
	};

	ns_engine::Camera *x;
	};

class bx2Collsion : public b2ContactListener {
	public:
		void BeginContact(b2Contact *contact);
		void EndContact(b2Contact *contact);
		void PreSolve(b2Contact *contact,
			      const b2Manifold *oldManifold);
		void PostSolve(b2Contact *contact,
			       const b2ContactImpulse *impulse);

	protected:
	private:
	};

const float timeStep = 1.f / 60.f;
const int velocityIterations = 20;
const int positionIterations = 20;

using ns_engine::Actor;
class bx2World {
public:
	bx2World() : world_(new b2World(b2Vec2(0, 0))),
		  bxColl_(new bx2Collsion),
		box2d_drive_(false)
	{
		world_->SetContactListener(bxColl_.get());
	}

	void SetGravity(const b2Vec2 &gravity)	{
		world_->SetGravity(gravity);
		box2d_drive_ = true;
	}

	void SetDbgDraw(ns_engine::Camera *x)	{
		bxdbg_ = make_shared<bx2DbgDraw>(x);
		bxdbg_->SetFlags(b2Draw::e_shapeBit);
		world_->SetDebugDraw(bxdbg_.get());
	}

	void RelateWorld(Actor *actor)
	{
		auto body = CreateBody(actor);
		relate_.insert(make_pair(actor, body));
	}

	void Update(const unsigned &dt)
	{
		world_->Step(timeStep, velocityIterations, positionIterations);
		world_->DebugDraw();

		UpdateElement();
	}

protected:
	b2Body *CreateBody(Actor *actor);
	void UpdateElement();

private:
	bool box2d_drive_;
	map<Actor *, b2Body *> relate_;
	shared_ptr<b2World> world_;
	shared_ptr<bx2DbgDraw> bxdbg_;
	shared_ptr<bx2Collsion> bxColl_;
};

class MainWorld:public bx2World,
	public single<MainWorld>
{
public:
protected:
private:
};

};
#endif
