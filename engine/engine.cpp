#include "engine.h"
#include "map.h"

using namespace ns_map;
namespace ns_engine {

void Game:: SwitchScene(const string &name) {
	cur_scene_->Reset();
	cur_scene_->SetAlive(false);
	cur_scene_ = FindSub(name);
	cur_scene_->SetAlive(true);
}

void Game::ShowLayer(const string &name, bool show, bool monopoly) {
	cur_scene_->ShowLayer(name, show, monopoly);
}

void Game::ListenMouse() {
	ns_sdl_winx::EventHandle::Instance()->SetMouseHandle(
		bind([](auto &&xx, auto &&yy, auto &&button, auto &&etype, Game *wx) { wx->GetCurScene()->OnMouse(xx, yy, etype, button); },
		     _1, _2, _3, _4, Game::Instance()));
}
void Game::ListenKey() {
	ns_sdl_winx::EventHandle::Instance()->SetKeyHandle(
		bind([](auto &&key, auto &&up_or_dow, Game *wx) { wx->GetCurScene()->OnKey(key, up_or_dow); }, _1, _2, Game::Instance()));
}

void Game::OnNotice(const string& layer, const string& actor, void* data) {
	auto pLayer = cur_scene_->FindSub(layer);
	//CHECK_POINT_VOID(pLayer);
	auto pActor = pLayer->FindSub(actor);
	//CHECK_POINT_VOID(pActor);
	pActor->OnNotice(data);
}


void Scene::ShowLayer(const string &name, bool show, bool monopoly) {
	auto layer = FindSub(name);
	layer->SetAlive(show);
	if (!show) {
		layer->Reset();
	}
	if (monopoly) {
		for (auto &it : sub_) {
			if (it != layer) {
				it->SetAlive(!show);
				if (!show) {
					it->Reset();
				}
			}
		}
	}
}

void Layer::Update(const unsigned &dt) {
	if (map_) {
		map_->CreateOrUpdateActor();
	}
	if (world_) {
		world_->Update(dt);
	}
	Temp<Layer, Actor>::Update(dt);
}

void Layer::CameraFollow(const int &delay, Actor *actor, bool center) {
	AddFrameEvent([=](void *self) {
		if (center) {
			auto [x, y] = actor->GetCenter();
			auto pThis = (Layer *)self;
			pThis->camera_->Follow(delay, x, y);
		} else {
			auto pThis = (Layer *)self;
			pThis->camera_->Follow(delay, actor->x_, actor->y_, false);
		}
	});
}
void Layer::RelateSub(Actor *sub) {
	if (world_) {
		world_->RelateWorld(sub);
	}
}
		 
bool Layer::Exist(Actor* actor) {
	auto it = find(sub_.begin(), sub_.end(), actor);
	return it != sub_.end();
}

void Layer::SetMap(ns_map::Map *map) {
	map_ = map;
	map->SetLayer(this);
}

}