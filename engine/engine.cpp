#include "engine.h"

namespace ns_engine {
void Layer::OnClick(const int &x, const int &y) {
	auto xx = x;
	auto yy = y;
	camera_->Trans(xx, yy);
	HandleClick(xx, yy);
}

void Layer::HandleClick(const int &xx, const int &yy) {
	for (auto &it : sub_) {
		if (it->OnClick(xx, yy)) {
			print("press", it);
		}
	}
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
}