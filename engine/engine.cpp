#include "engine.h"
#include "map.h"

using namespace ns_map;
using namespace ns_sdl_ast;
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

Layer *Game::GetLayer(const string &layer) {
	return cur_scene_->FindSub(layer);
}

void Game::AlphaOver(const int &x, const int &y, const int &w, const int &h) {
	auto path = GetPic("alpha.png");
	auto texture = AssetMgr::Instance()->GetTexture(path);
	FillRect(x, y, w, h, texture);
}


bool Game::GetDieFlag(int &x, int &y) {
	if (use_role_) {
		if (leadrol_) {
			x = leadrol_->x_, y = leadrol_->y_;
			return true;
		} else {
			return false;
		}
	} else {
		if (camera_) {
			camera_->GetCenter(x, y);
			return true;
		} else {
			return false;
		}
	}
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

void Layer::RemoveDeath() {
	for (auto it = sub_.begin(); it != sub_.end();) {
		if ((*it)->IsDeath()) {
			if (world_) {
				world_->Destroy(*it);
			}
			(*it)->Destroy();
			it = sub_.erase(it);
		} else {
			it++;
		}
	}
}

void Layer::ForceClear() {
	for (auto &it : sub_) {
		it->Death();
	}
	RemoveDeath();
}

void Layer::Update(const unsigned &dt) {
	if (map_) {
		map_->CreateOrUpdateActor();
	}
	if (world_) {
		world_->Update(dt);
	}
	Temp<Layer, Actor>::Update(dt);

	RemoveDeath();

}

bool Layer::IsEnd() {
	return map_->IsEnd();
}

//void Layer::Reset() {
//	
//}


void Actor::Update(const unsigned &dt) {
	auto &[x, y] = vel_;
	x_ += x, y_ += y;
	AutoDie(); //死亡检测在ai驱动之前，可用死亡检测判断能否操作对象
	AiDrive();
	Temp<Actor, Actor>::Update(dt);
}

void Actor::AiDrive() {
	if (!ai_quene_.empty()) {
		auto ai = ai_quene_.front();
		if (ai.second->IsDeath() ||ai.first==nullptr|| ai.first->Drive(this)) {
			ai_quene_.pop();
		}
		return;
	}
	if (!ai_chain_.alive) {
		ai_chain_.alive = ai_chain_.chain.empty() ? nullptr : (ai_chain_.chain[0]);
	}
	if (ai_chain_.alive && ai_chain_.alive->Drive(this)) {
		SwitchAi();
	}
}

void Actor::AutoDie() {
	if (!parent_->auto_clear_sub_) {
		return;
	}
	int x, y;
	if (!Game::Instance()->GetDieFlag(x, y)) {
		return;
	}

	auto w = parent_->w_;
	auto h = parent_->h_;
	auto xn = ns_params::active_distance_ * autodie_;
	w *= xn;
	h *= xn;
	auto x_dt = abs(x_ - x);
	auto y_dt = abs(y_ - y);
	if (!(x_dt < w && y_dt < h)) {
		is_death_ = true;
	}
}

void Layer::CameraFollow(const int &delay, Actor *actor, bool center) {
	AddFrameEvent([=](void *self) {
		auto pThis = (Layer *)self;
		if (center) {
			auto [x, y] = actor->GetCenter();
			pThis->camera_->Follow(delay, x, y);
		} else {
			pThis->camera_->Follow(delay, actor->x_, actor->y_, false);
		}
	});
}

void Layer::CameraFollow(const int &delay, const string &name, bool center) {
	AddFrameEvent([=](void *self) {
		auto pThis = (Layer *)self;
		auto actor = pThis->GetActor(name);
		if (!actor) {
			return;
		}
		if (center) {
			auto [x, y] = actor->GetCenter();
			pThis->camera_->Follow(delay, x, y);
		} else {
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

default_random_engine *common_random_engine = nullptr;
default_random_engine &re() {
	if (!common_random_engine) {
		random_device rd;
		common_random_engine = new default_random_engine{rd()};
	}
	return *common_random_engine;
}

}