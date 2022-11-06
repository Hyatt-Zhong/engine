#include "sdl_windx.h"

namespace ns_engine {
	enum AssetType {
		Texture,
		Sound,
	};
	using frame_event = function<void()>;
	template<typename T, typename S>
	class Temp
	{
	public:
		Temp() :alive_(true), curr_anim_(nullptr) {}
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
				auto [x, y, w, h] = SDL_Rect({ x_,y_,w_,h_ });
				winx_->Offset(x, y, w, h);
				//print(x, y, w, h);
				curr_anim_->Update(dt, x, y, w, h);
			}
			Sort();
			for (auto& it : sub_) {
				if (it->alive_) {
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
			sub->winx_ = winx_;
			sub_.push_back(sub);
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
			winx_->Offset(x_, y_, 0, 0);
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
				it();
			}
		}
		void AddFrameEvent(const frame_event &fe){
			frame_events_.push_back(fe);
		}

		vector<S*> sub_;
		bool alive_;
		int zorder_;

		int x_, y_;
		int w_, h_;
		string name_;

		map<int, ns_sdl_img::Animation> anims_;
		ns_sdl_img::Animation* curr_anim_;
		ns_sdl_winx::windowx* winx_;
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
		void Create(const int& w, const int& h) {
			WinInit();
			windowx::Create(w, h);
			winx_ = this;
			ns_sdl_img::AssetMgr::Instance()->SetRenderer(render_);
		}

		void Draw(const unsigned& dt) {
			Update(dt);
		}

	protected:
	private:
	};

	class Layer;
	class Scene :public Temp<Scene, Layer>
		, public Base<Game>
	{
	public:
		//void Update(const unsigned& dt);
		Scene() {
			x_ = y_ = 0;
			w_ = 500, h_ = 400;
		}
	protected:
	private:
	};

	class Actor;
	class Layer :public Temp<Layer, Actor>
		, public Base<Scene>
	{
	public:
		//void Update(const unsigned& dt);
		Layer() {
			x_ = y_ = 0;
			w_ = 300, h_ = 200;
		}
	protected:
	private:
	};

	class Actor :public Temp<Actor, Actor>
		, public Base<Layer>
	{
	public:
		//void Update(const unsigned& dt);
		Actor() {
			SetName("actor");
			x_ = y_ = 0;
			w_ = 50, h_ = 40;
		}
	protected:
	private:
	};

}
