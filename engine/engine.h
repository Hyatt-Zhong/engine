#ifndef ENGINE_H
#define ENGINE_H

#include "sdl_windx.h"
#include "bx2-engine.h"

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
		Camera(ns_sdl_winx::windowx *win)
			: win_(win), x_(0), y_(0), w_(0), h_(0){}
		void SetPostion(const int &x, const int &y) {
			x_ = x, y_ = y; 
			ChangeCenter();
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

		void AddSub(S* sub) {
			sub->SetParent((S::Parent*)this);
			sub->camera_ = camera_;
			sub_.push_back(sub);
		}
		void SetCamera(Camera *camera) { 
			camera_ = camera;
			for (auto &it : sub_) {
				it->SetCamera(camera_);
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
				it->second.ts_.push_back(tp);
			}
			else {
				tuple<string, SDL_Texture*, int> tp = { path,texture,dt };
				ns_sdl_img::Animation ani(ns_sdl_img::AssetMgr::Instance()->GetRenderer());
				ani.ts_.push_back(tp);
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

		virtual bool IsAlive() { return true; }

		vector<S*> sub_;
		int zorder_;

		int x_, y_;
		int w_, h_;
		string name_;

		map<int, ns_sdl_img::Animation> anims_;
		ns_sdl_img::Animation* curr_anim_;
		Camera* camera_;
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
			camera_ = new Camera(this);
			camera_->SetPostion(0, 0);
			camera_->SetViewport(w, h);
			ns_sdl_img::AssetMgr::Instance()->SetRenderer(render_);

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

		Camera* MainCamera() { return camera_; }

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
	protected:
	private:
	};

	class Actor :public Temp<Actor, Actor>
		, public Base<Layer>
	{
	public:
		Actor() : is_click_(false) {}
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

		bool OnClick(const int &x, const int &y) {
			if (x_ < x && y_ < y && x_ + w_ > x && y_ + h_ > y) {
				is_click_ = true;
			} else {
				is_click_ = false;
			}
			return is_click_;
		}

		bool IsClick() { return is_click_; }
	public:
	protected:
	private:
		bool is_click_;
	};
	
	};
#endif // !ENGINE_H
