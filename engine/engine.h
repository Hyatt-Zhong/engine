#ifndef ENGINE_H
#define ENGINE_H

#include "sdl_windx.h"
#include "bx2-engine.h"
#include "data-def.h"
#include "macro-def.h"
#include "ai-base.h"

namespace ns_module {
	using namespace std;
MTYPE_BEG(mod_type)
	MTYPE(none)
	MTYPE(building)
	MTYPE(enemy)
	MTYPE(boss)
	MTYPE(npc)
	MTYPE(role)
	MTYPE(tree)
	MTYPE(weapon)
	MTYPE(bullet)
	MTYPE(skill)
	MTYPE(effect)
	MTYPE(combination)
MTYPE_END

using typeset = bitset<32>;
}

namespace ns_map {
class Map;
}
namespace ns_engine {
using namespace std;
using namespace ns_ai;
using namespace placeholders;

namespace ns_params {
static float render_distance_ = 3 / 2;//3个视野内
static float active_distance_ = 0.55; //1个多一点视野内

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
		void Trans(int &x, int &y,const int& h) {
			win_->Offset(x, y, 0, h);
			x += x_, y += y_;
		}

		void GetCenter(int& cx, int& cy) { cx = center_x_;
			cy = center_y_;
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

	struct Center {
		int x, y;
	};

	using frame_event = function<void(void*)>;
	//using frame_event = void(*)(void*);
	template<typename T, typename S>
	class Temp
	{
	public:
		using ThisClass = Temp<T, S>;
		Temp() : curr_anim_(nullptr)
			, x_(0), y_(0), w_(0), h_(0),angle_(0) {}
		virtual ~Temp() {}
		/*virtual void LoadAsset() {
			for (auto& it : anims_) {
				it.second.LoadAsset();
			}
			for (auto& it : sub_) {
				it->LoadAsset();
			}
		}*/
		virtual void Update(const unsigned& dt) {
			FrameEvent();
			ChangeState();

			{
				int x, y;
				if (camera_->Catch(this, x, y)) {
					if (curr_anim_) {
						curr_anim_->Update(dt, x, y, w_, h_);
					}
					if (InRange()) {
						OnHover(x, y, w_, h_);
					}
				}
			}
			
			Sort();
			for (auto& it : sub_) {
				if (it->IsAlive()) {
					it->Update(dt);
				}
			}
			FrameEventOnce();
		}

		virtual void ChangeState() {
			auto it = anims_.begin();
			if (it != anims_.end()) {
				curr_anim_ = &it->second;
			}
		}

		virtual void AddSub(S *sub) {
			sub->SetParent((S::Parent*)this);
			sub->camera_ = camera_;
			sub->world_ = world_;
			sub_.push_back(sub);
		}
		void DeleteSub(S* sub) { 
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

		void AddAssetAnimation(const string& path_, const int& dt, const int& state)
		{
			auto path = Game::Instance()->GetPic(path_);
			auto texture = ns_sdl_ast::AssetMgr::Instance()->GetTexture(path);
			auto it = anims_.find(state);
			if (it != anims_.end()) {
				tuple<string, SDL_Texture*, int> tp = { path,texture,dt };
				it->second.GetTs().push_back(tp);
			}
			else {
				tuple<string, SDL_Texture*, int> tp = { path,texture,dt };
				auto rr = ns_sdl_ast::AssetMgr::Instance()->GetRenderer();
				ns_sdl_ast::Animation<ThisClass> ani(rr, tp, this);
				anims_.insert(make_pair(state, ani));
			}
		}

		void LoadMusic(const string &name) { 
			auto path = Game::Instance()->GetMus(name);
			ns_sdl_ast::AssetMgr::Instance()->GetMusic(name, path);
		}

		void LoadChunk(const string &name) {
			auto path = Game::Instance()->GetMus(name);
			ns_sdl_ast::AssetMgr::Instance()->GetChunk(name, path);
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

		S *FindSub(const string &name) {
			auto it = find_if(sub_.begin(), sub_.end(), [name](auto &&sub) { return sub->name_ == name; });
			if (it != sub_.end()) {
				return *it;
			}
			return nullptr;
		}

		void FrameEvent() {
			for (auto& it:frame_events_){
				it(this);
			}
		}
		void AddFrameEvent(const frame_event &fe){
			frame_events_.push_back(fe);
		}

		void FrameEventOnce() {
			for (auto &it : frame_events_once_) {
				it(this);
			}
			frame_events_once_.swap(vector<frame_event>());
		}
		void AddFrameEventOnce(const frame_event &fe) { frame_events_once_.push_back(fe); }

		Center GetCenter() {
			return {x_ + w_ / 2, y_ + h_ / 2};
		}

		d_vel GetCenterB2() { return move(d_vel(x_ + w_ / 2, y_ + h_ / 2));
		}

		virtual bool IsAlive() { return alive_; }
		void SetAlive(bool alive) { alive_ = alive; }
		void InitAlive(bool alive) {
			alive_ = alive;
			init_alive_ = alive;
		}

		SizeInfo GetInfo() {
			return SizeInfo(x_, y_, w_, h_);
		}

		bool InRange(const int &x, const int &y) {
			if (x_ < x && y_ < y && x_ + w_ > x && y_ + h_ > y) {
				in_range_ = true;
			} else {
				in_range_ = false;
			}
			return in_range_;
		}
		bool InRange() { return in_range_; }

		virtual void OnHover(const int &x, const int &y, const int &w, const int &h) {}

		virtual void Reset() {
			angle_ = 0;
			in_range_ = false;
			alive_ = init_alive_;
			for (auto &it : sub_) {
				it->Reset();
			}
		}

		void PlayMusic(const string &name) { name.empty() ? void(0) : ns_sdl_ast::AssetMgr::Instance()->PlayMusic(name); }
		void PlayChunk(const string &name) { name.empty() ? void(0) : ns_sdl_ast::AssetMgr::Instance()->PlayChunk(name); }

		vector<S*> sub_;
		int zorder_;

		int x_, y_;
		int w_, h_;
		string name_;

		map<int, ns_sdl_ast::Animation<ThisClass>> anims_;
		ns_sdl_ast::Animation<ThisClass> *curr_anim_;
		double angle_;

		Camera *camera_ = nullptr;
		ns_box2d::bx2World *world_ = nullptr;
		vector<frame_event> frame_events_;
		vector<frame_event> frame_events_once_;

	private:
		bool alive_ = true;
		bool init_alive_ = true;

	private:
		bool in_range_ = false;
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
		Game() : cur_scene_(nullptr), leadrol_(nullptr) {}
	public:
		void Create(const int& w, const int& h, const void *wind = nullptr) {
			w_ = w, h_ = h;
			windowx::WinInit();
			windowx::Create(w, h, wind);
			camera_ = MainCamera::Instance();
			camera_->SetWindow(this);
			camera_->SetPostion(0, 0);
			camera_->SetViewport(w, h);
			ns_sdl_ast::AssetMgr::Instance()->SetRenderer(render_);

			//world_ = ns_box2d::MainWorld::Instance();
			//AddWorld(ns_box2d::MainWorld::Instance());
		}

		void Draw(const unsigned& dt) {
			/*for (auto& it:worlds_){
				it->Update(dt);
			}*/
			Update(dt);
			for (auto &it : over_draw_) {
				it();
			}
			over_draw_.clear();
		}

		//void AddWorld(ns_box2d::bx2World *w) { worlds_.push_back(w); }
		bool IsAlive() { return true; }

		void Destroy() { 
			SAFE_DELETE(camera_);
		}

		Actor *Leadrol() { return leadrol_; }
		bool GetDieFlag(int &x, int &y);
		void SetDieFlag(bool useRole) { use_role_ = useRole; }

		void SetLeadrol(Actor *rol) { leadrol_ = rol; }

		void SwitchScene(const string &name);
		void ShowLayer(const string &name, bool show, bool monopoly = true);

		void OnNotice(const string &layer, const string &actor, void *data);
		Layer *GetLayer(const string &layer);

		void SetPath(const string &path) {
			music_path_ = path + "\\asset\\music\\";
			pic_path_ = path + "\\asset\\pic\\";
			module_path_ = path + "\\module\\";
			map_path_ = path + "\\map\\";
		}

		string GetModulePath() { return module_path_; }
		string GetMapPath() { return map_path_; }

		Scene* GetCurScene() {
			auto it = find_if(sub_.begin(), sub_.end(), [](auto &&xx) { return xx->IsAlive(); });
			cur_scene_ = it != sub_.end() ? *it : nullptr;
			return cur_scene_;
		}

		string GetPic(const string &path) { return pic_path_ + path; }
		string GetMus(const string &path) { return music_path_ + path; }
		void AlphaOver(const int &x, const int &y, const int &w, const int &h);

		using OverDrawFn = function<void()>;
		void OverDraw(OverDrawFn od) { over_draw_.push_back(od); }

		void ReleaseAsset() { ns_sdl_ast::AssetMgr::Instance()->ReleaseAsset(); }
	public:
		static void ListenMouse();
		static void ListenKey();

	protected:
		Scene *cur_scene_;
		bool use_role_ = true;
		vector<OverDrawFn> over_draw_;

	private:
		vector<ns_box2d::bx2World *> worlds_;
		Actor *leadrol_;
		string music_path_;
		string pic_path_;
		string module_path_;
		string map_path_;
	};

	class Layer;
	class Scene : public Temp<Scene, Layer>, public Base<Game>, public ns_sdl_winx::Input<Scene> {
	public:
		Scene() { InitAlive(false); }
		Scene(const string &name) {
			name_ = name;
			InitAlive(false);
		}
		void ShowLayer(const string &name, bool show, bool monopoly = true);

	protected:
	private: 
		
	};

	class Actor;
	class Layer : public Temp<Layer, Actor>, public Base<Scene>, public ns_sdl_winx::Input<Layer> {
	public:
		Layer() {}
		Layer(const string &name) { name_ = name; }

		void CameraFollow(const int &delay, Actor *x, bool center = true);
		void CameraFollow(const int &delay, const string& name, bool center = true);
		void RelateSub(Actor *sub);
		bool Exist(Actor *actor);
		void Update(const unsigned &dt);
		void SetMap(ns_map::Map *map);
		void RemoveDeath();
		void ForceClear();
		ns_map::Map* GetMap() { return map_; }
		bool IsEnd();
		/*void Reset();*/

	public:
		void AddToQuickMap(const string &name, Actor *actor) { quick_map[name] = actor; }
		Actor *GetActor(const string &name) { MAP_FIND(quick_map, name, nullptr);	}
		void DeleteFromQuickMap(Actor *actor) {
			for (auto &it : quick_map) {
				if (it.second == actor) {
					quick_map.erase(it.first);
					return;
				}
			}
		}

	public:
		bool auto_clear_sub_ = true;//自动清理对象，但有的不需要自动清理，比如菜单层
	protected:
		ns_map::Map *map_ = nullptr;
	private:
		map<string, Actor*> quick_map;
	};

	class Actor : public Temp<Actor, Actor>, public Base<Layer>, public ns_sdl_winx::Input<Actor> {
	public:
		Actor() : is_death_(false), vel_(0, 0), create_body_(nullptr),
			type_(ns_module::mod_type::none), goaltype_(0),direct_(0.,1.) {}
		void AutoDie();

		void SetVel(const d_vel &v) {
			auto bx2_drive = ns_box2d::MainWorld::Instance()->IsBx2Drive();
			if (bx2_drive) {
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

		void Update(const unsigned &dt);

		d_vel GetVel() {
			auto bx2_drive = ns_box2d::MainWorld::Instance()->IsBx2Drive();
			if (bx2_drive) {
				return bx2_vel_;
			} else {
				return vel_;
			}
		}

		void SetDirect(const d_vel &direct) { direct_ = direct; }

		virtual void OnNotice(void* data) {}

		virtual void OnCollision(Actor *actor) {
			print("collision with", actor);
		}

		void Death() { is_death_ = true; }
		void BeKill() {
			is_death_ = true;
			be_kill_ = true;
		}
		bool IsBeKill() { return be_kill_; }
		bool IsDeath() { return is_death_; }
		void Destroy() { is_destroy_ = true; }
		virtual bool IsDestroy() { return is_destroy_; }

	public:
		using CreateBodyFunc = b2Body* (*)(Actor*);
		Actor(const CreateBodyFunc &create_body) : is_death_(false), vel_(0, 0),
			create_body_(create_body), direct_(0., 1.) {}
		void SetCreateBodyFunc(CreateBodyFunc func) { create_body_ = func; }
		CreateBodyFunc create_body_;

	public:
		void PushAi(Ai* ai) { 
			ai->SetMaster(this);
			ai_chain_.chain.push_back(ai);
		}
		void SwitchAi() {
			if (!ai_chain_.alive) {
				ai_chain_.alive = ai_chain_.chain.empty() ? nullptr : (ai_chain_.chain[0]);
				ai_chain_.index = 0;
				return;
			}
			ai_chain_.index++;

			if (ai_chain_.index >= ai_chain_.chain.size()) {
				ai_chain_.alive = (ai_chain_.chain[0]);
				ai_chain_.index = 0;
			} else {
				ai_chain_.alive = (ai_chain_.chain[ai_chain_.index]);
			}
		}

		MultAi *FindContorlAi(const string &name) { MAP_FIND(ai_control_, name, nullptr) }

		void PushAi(MultAi *mai, Actor *master) { ai_quene_.push(make_pair(mai, master)); }

	public:
		d_vel direct_;
		
	protected:
		float autodie_ = 1.f;
		bool is_death_;
		bool be_kill_ = false;
		bool is_destroy_ = false;
		d_vel vel_;
		d_vel bx2_vel_;
		
		AiChain ai_chain_;//自己的ai
		AiQuene ai_quene_;//别人的ai
		AiContorl ai_control_;//控制别人的ai
	protected:
		void AiDrive();

	public:
		ns_module::typeset type_ = 0;							//本身类型
		ns_module::typeset goaltype_ = 0;                      //敌对的类型
	private:
	};

	default_random_engine &re();
	
	};
#endif // !ENGINE_H
