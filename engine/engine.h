#ifndef ENGINE_H
#define ENGINE_H

#include "sdl_windx.h"
#include "bx2-engine.h"
#include "data-def.h"

namespace ns_engine {
using namespace std;
namespace ns_params {
static float render_distance_ = 3 / 2;//3个视野内
static float active_distance_ = 0.75;//1个多一点视野内

};
	enum AssetType {
		Texture,
		Sound,
	};

	class Camera {
	public:
		Camera() {}
		void SetWindow(ns_sdl_winx::windowx *win) { win_ = win; }

		Camera(ns_sdl_winx::windowx *win)
			: win_(win), x_(0), y_(0), w_(0), h_(0){}
		void SetPostion(const int &x, const int &y, bool center = true) {
			if (center) {
				center_x_ = x;
				center_y_ = y;
				x_ = center_x_ - w_ / 2;
				y_ = center_y_ - h_ / 2;
			} else {
				x_ = x, y_ = y;
				ChangeCenter();
			}
		}

		void Follow(const int& delay, const int &x, const int &y, bool center = true) {
			if (delay == 0) {
				SetPostion(x, y, center);
			} else {
				pos_line_.push(make_pair(x, y));
				if (pos_line_.size() >= delay) {
					auto [xx, yy] = pos_line_.front();
					pos_line_.pop();
					SetPostion(xx, yy, center);
				}
			}
		}

		void SetViewport(const int& w, const int& h) {
			w_ = w, h_ = h;
			ChangeCenter();
		}

		void Move(const int &dx, const int &dy) {
			x_ += dx, y_ += dy; 
			ChangeCenter();
		}

		void CenterMoveToByMouse(const int& x, const int& y) {
			auto m = x;
			auto n = y;
			Trans(m, n);
			CenterMoveTo(m, n);
		}

		void CenterMoveTo(const int& x, const int& y) {
			center_x_ = x, center_y_ = y;
			x_ = center_x_ - w_ / 2;
			y_ = center_y_ - h_ / 2;
		}

		template<typename T> 
		bool Catch(T *tx, int &x, int &y){
			auto ret = false;
			auto x_dt = abs(tx->x_ - center_x_);
			auto y_dt = abs(tx->y_ - center_y_);
			if (x_dt < w_ * ns_params::render_distance_ &&
			    y_dt < h_ * ns_params::render_distance_) {
				ret = true;
			}
			x = tx->x_, y = tx->y_;
			x -= x_, y -= y_;
			win_->Offset(x, y, tx->w_, tx->h_);
			return ret;
		}

		void Trans(int &x, int &y) {
			win_->Offset(x, y, 0, 0);
			x += x_, y += y_;
		}

	public:
		ns_sdl_winx::windowx *win_;
	protected:
		void ChangeCenter() {
			center_x_ = x_ + w_ / 2;
			center_y_ = y_ + h_ / 2;
		}
	private:
		int x_, y_, w_, h_;
		int center_x_, center_y_;
		std::queue<pair<int, int>> pos_line_;
	};

	class MainCamera : public Camera, 
		public single<MainCamera> 
	{
	public:
	protected:
	private:
	};

	struct SizeInfo {
		float x_, y_, w_, h_;
		SizeInfo(int x, int y, int w, int h) : x_(x), y_(y), w_(w), h_(h) {} 
		void div(const int &nx) { x_ /= nx, y_ /= nx, w_ /= nx, h_ /= nx; }
	};

	using frame_event = function<void(void*)>;
	template<typename T, typename S>
	class Temp
	{
	public:
		Temp() : curr_anim_(nullptr)
			, x_(0), y_(0), w_(0), h_(0) {}
		~Temp() {}
		virtual void Init() {}
		virtual void LoadAsset() {
			for (auto& it : anims_) {
				it.second.LoadAsset();
			}
			for (auto& it : sub_) {
				it->LoadAsset();
			}
		}
		virtual void Update(const unsigned& dt) {
			FrameEvent();
			ChangeState();

			if (curr_anim_) {
				int x, y;
				if (camera_->Catch(this, x, y)) {
					curr_anim_->Update(dt, x, y, w_, h_);
				}
			}
			Sort();
			for (auto& it : sub_) {
				if (it->IsAlive()) {
					it->Update(dt);
				}
			}
		}

		virtual void ChangeState() {
			auto it = anims_.begin();
			if (it != anims_.end()) {
				curr_anim_ = &it->second;
			}
		}
		virtual void Uninit() {}

		void AddSub(S *sub) {
			sub->SetParent((S::Parent*)this);
			sub->camera_ = camera_;
			sub->world_ = world_;
			sub_.push_back(sub);
		}
		virtual void DeleteSub(S* sub) { 
			for (auto it = sub_.begin(); it != sub_.end();) {
				if (*it == sub) {
					it = sub_.erase(it);
				} else {
					it++;
				}
			}
		}
		void SetCamera(Camera *camera) { 
			camera_ = camera;
			for (auto &it : sub_) {
				it->SetCamera(camera_);
			}
		}

		void SetWorld(ns_box2d::bx2World *world) {
			world_ = world;
			for (auto &it : sub_) {
				it->SetWorld(world_);
			}
		}

		void Sort() {
			auto& ret = sub_;
			std::sort(ret.begin(), ret.end(), [](auto&& a, auto&& b) {
				return a->zorder_ < b->zorder_; });
		}

		void AddAssetAnimation(const string& path, const int& dt, const int& state)
		{
			auto texture = ns_sdl_img::AssetMgr::Instance()->GetTexture(path);
			auto it = anims_.find(state);
			if (it != anims_.end()) {
				tuple<string, SDL_Texture*, int> tp = { path,texture,dt };
				it->second.GetTs().push_back(tp);
			}
			else {
				tuple<string, SDL_Texture*, int> tp = { path,texture,dt };
				auto rr = ns_sdl_img::AssetMgr::Instance()->GetRenderer();
				ns_sdl_img::Animation ani(rr, tp);
				anims_.insert(make_pair(state, ani));
			}
		}

		void SetPostionByMouse(const int& x, const int& y, bool center = true) {
			x_ = x, y_ = y;
			camera_->Trans(x_, y_);
			if (center) {
				x_ = x_ - w_ / 2, y_ = y_ - h_ / 2;//x,y 为对象的中点
			}
		}
		void SetPostion(const int& x, const int& y, bool center = true) {
			x_ = x, y_ = y;
			if (center) {
				x_ = x_ - w_ / 2, y_ = y_ - h_ / 2;//x,y 为对象的中点
			}
		}
		void SetSize(const int& w, const int& h) {
			w_ = w, h_ = h;
		}
		void SetName(const string& name) {
			name_ = name;
		}

		void FrameEvent() {
			for (auto& it:frame_events_){
				it(this);
			}
		}
		void AddFrameEvent(const frame_event &fe){
			frame_events_.push_back(fe);
		}

		pair<int, int> GetCenter() {
			return {x_ + w_ / 2, y_ + h_ / 2};
		}

		virtual bool IsAlive() { return true; }

		SizeInfo GetInfo() {
			return SizeInfo(x_, y_, w_, h_);
		}

		vector<S*> sub_;
		int zorder_;

		int x_, y_;
		int w_, h_;
		string name_;

		map<int, ns_sdl_img::Animation> anims_;
		ns_sdl_img::Animation* curr_anim_;
		Camera* camera_;
		ns_box2d::bx2World *world_;
		vector<frame_event> frame_events_;
	};

	template<typename T>
	class Base
	{
	public:
		using Parent = T;
		void SetParent(Parent* parent) {
			parent_ = parent;
		}
		Parent* parent_ = nullptr;
	};

	class Scene;
	class Game :public ns_sdl_winx::windowx,
		public Temp<Game, Scene>,
		public single<Game>
	{
	public:
		void Create(const int& w, const int& h, const void *wind = nullptr) {
			windowx::WinInit();
			windowx::Create(w, h, wind);
			camera_ = MainCamera::Instance();
			camera_->SetWindow(this);
			camera_->SetPostion(0, 0);
			camera_->SetViewport(w, h);
			ns_sdl_img::AssetMgr::Instance()->SetRenderer(render_);

			world_ = ns_box2d::MainWorld::Instance();
			AddWorld(ns_box2d::MainWorld::Instance());
		}

		void Draw(const unsigned& dt) {
			for (auto& it:worlds_){
				it->Update(dt);
			}
			Update(dt);
		}

		void AddWorld(ns_box2d::bx2World *w) { worlds_.push_back(w); }

		void Destroy() { 
			SAFE_DELETE(camera_);
		}

		Actor *Leadrol() { return leadrol_; }
		void SetLeadrol(Actor *rol) { leadrol_ = rol; }

	protected:
	private:
		vector<ns_box2d::bx2World *> worlds_;
		Actor *leadrol_;
	};

	class Layer;
	class Scene :public Temp<Scene, Layer>
		, public Base<Game>
	{
	public:
		Scene() {}
	protected:
	private:
	};

	class Actor;
	class Layer :public Temp<Layer, Actor>
		, public Base<Scene>
	{
	public:
		Layer() {}

		void OnClick(const int &x, const int &y);
		virtual void HandleClick(const int &x, const int &y);
		void CameraFollow(const int &delay, Actor *x, bool center = true);
		void DeleteSub(Actor *sub) override;
		void RelateSub(Actor *sub);
	protected:
	private:
	};

	class Actor :public Temp<Actor, Actor>
		, public Base<Layer>
	{
	public:
		Actor() : is_click_(false), is_destroy_(false), vel_(0, 0), create_body_(nullptr) {}
		bool Active() {
			auto leadrol = Game::Instance()->Leadrol();
			auto x = leadrol->x_;
			auto y = leadrol->y_;
			auto w = leadrol->w_;
			auto h = leadrol->h_;
			w *= ns_params::active_distance_;
			h *= ns_params::active_distance_;
			auto x_dt = abs(x_ - x);
			auto y_dt = abs(y_ - y);
			if (x_dt < w  &&
			    y_dt < h ) {
				return true;
			}

			return false;
		}

		void Update(const unsigned &dt) {
			auto &[x, y] = vel_;
			x_ += x, y_ += y;
			Temp<Actor, Actor>::Update(dt);
			if (is_destroy_)
			{
				world_->Destroy(this);
				parent_->DeleteSub(this);
			}
		}

		void SetVel(const d_vel &v) {
			auto bx2_drive = ns_box2d::MainWorld::Instance()->IsBx2Drive();
			if (bx2_drive)
			{
				auto body = ns_box2d::MainWorld::Instance()->GetBody(this);
				//body->ApplyForceToCenter(v, true);//设置作用力				
				if (bx2_vel_ != v) {
					body->SetLinearVelocity(v);
					bx2_vel_ = v;
				}
			} else {
				vel_ = v;
			}
		}

		bool OnClick(const int &x, const int &y) {
			if (x_ < x && y_ < y && x_ + w_ > x && y_ + h_ > y) {
				is_click_ = true;
			} else {
				is_click_ = false;
			}
			return is_click_;
		}

		bool IsClick() { return is_click_; }

		virtual void OnCollision(Actor *actor) {
			print("collision with", actor);
		}
	public:
		using CreateBodyFunc = b2Body* (*)(Actor*);
		Actor(const CreateBodyFunc &create_body) : is_click_(false), is_destroy_(false),
			vel_(0, 0), create_body_(create_body) {}
		void SetCreateBodyFunc(CreateBodyFunc func) { create_body_ = func; }
		CreateBodyFunc create_body_;

	protected:
		bool is_destroy_;
		bool is_click_;
		d_vel vel_;
		d_vel bx2_vel_;
	private:
	};

	class xActor : public Actor
	{
	public:
		using Actor::is_destroy_;
	protected:
	private:
	};
	
	};
#endif // !ENGINE_H
