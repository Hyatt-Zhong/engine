#ifndef SDL_WINDX_H
#define SDL_WINDX_H
#include <windows.h>
#include <SDL/include/SDL.h>
#include <SDL/include/SDL_image.h>
#include <SDL/include/SDL_mixer.h>
#include <common.h>
#include <mutex>

using renderer = SDL_Renderer;
namespace ns_sdl_winx {
	using namespace std;
	const unsigned frame_interval = 13;
	const unsigned key_count = 32;

	using key_handle = function<void(const SDL_Keycode &key, const Uint32 &up_or_dow)>;
	using key_event = function<void(const SDL_Keycode &key, const Uint32 &up_or_dow, void *data)>;
	using mouse_handle = function<void(const int& x, const int& y, const Uint8& button, const Uint32 &e_type)>;
	using event_handle = function<void(const SDL_Event& e)>;
	class EventHandle :public single<EventHandle>
	{
	public:
		EventHandle() {
			key_state_ = SDL_GetKeyboardState(NULL);
		}

		void AddEventHandle(Uint32 key, event_handle hdl) {
			if (event_handle_.find(key) == event_handle_.end()) {
				event_handle_.insert(make_pair(key, hdl));
			}
		}
		void SetMouseHandle(mouse_handle hdl) { mhdl = hdl; }
		void SetKeyHandle(key_handle hdl) { khdl = hdl; }

		bool GetKeyState(const SDL_Scancode &key){
			return key_state_[key];
		}

		//获取鼠标按钮是否按下状态
		//mouse index 1:left 2:middle 3:right
		bool GetMouseState(const int &index, int *x = nullptr, int *y = nullptr) {
			return SDL_GetMouseState(x, y) & SDL_BUTTON(index);
		}

		void OnEvent(const SDL_Event& e) {
			switch (e.type) {
			case SDL_KEYUP:
			case SDL_KEYDOWN: {
				khdl(e.key.keysym.sym, e.type);
			} break;
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEWHEEL: {
				mhdl(e.button.x, e.button.y, e.button.button, e.type);
			} break;
			default: {
				auto it = event_handle_.find(e.type);
				if (it != event_handle_.end()) {
					it->second(e);
					break;
				}
			} break;
			}
		}
		
	protected:
	private:
		mouse_handle mhdl;
		key_handle khdl;
		
		map<Uint32, event_handle> event_handle_;

		const Uint8 *key_state_ = nullptr;
	};

	template<typename T> 
	class Input {
	public:
		virtual void OnMouse(const int &x, const int &y, const Uint32 &e_type, const Uint8 &button) {
			switch (e_type) {
			case SDL_MOUSEBUTTONUP: {
				if (button == 1) {
					OnLMouseUp(x, y);
				} else if (button == 3) {
					OnRMouseUp(x, y);
				}
			} break;
			case SDL_MOUSEMOTION: {
				OnMouseMove(x, y);
			} break;
			default: {
				for (auto &it : ((T*)(this))->sub_) {
					it->OnMouse(x, y, e_type, button);
				}
			} break;
			}
		}
		virtual void OnKey(const SDL_Keycode &key, const Uint32 &up_or_down) {
			auto it = kevent.find(key);
			if (it != kevent.end()) {
				it->second(key, up_or_down, (T *)this);//必须做这个转换成T*的操作，否则虽是同一个指针，但是在回调函数中无法正确转换
				return;
			}
			if (((T *)(this))->sub_.empty()) {
				return;
			}
			for (auto &it : ((T*)(this))->sub_) {
				if (it->IsAlive())
					it->OnKey(key, up_or_down);
			}
		}
		virtual void OnMouseMove(const int &x, const int &y) {
			for (auto &it : ((T*)(this))->sub_) {
				if (it->IsAlive())
					it->OnMouseMove(x, y);
			}
		}
		virtual void OnLMouseUp(const int &x, const int &y) {
			for (auto &it : ((T *)(this))->sub_) {
				if (it->IsAlive())
					it->OnLMouseUp(x, y);
			}
		}
		virtual void OnRMouseUp(const int &x, const int &y) {
			for (auto &it : ((T*)(this))->sub_) {
				if (it->IsAlive())
					it->OnRMouseUp(x, y);
			}
		}
		//virtual void OnMMouse(const int &x, const int &y) {}

		void AddKeyEvent(const SDL_Keycode &key, const key_event &hdl) { kevent[key] = hdl; }
		void RemoveKeyEvent(const SDL_Keycode &key) { kevent.erase(key); }

	protected:
	private:
		map<SDL_Keycode, ns_sdl_winx::key_event> kevent;
	};
	class windowx
	{
	public:
		windowx() :win_(nullptr), render_(nullptr), last_tick_(0), quit_(false),
			ox_(0), oy_(0), dt_(0)
		{}
		void WinInit() {
			SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		}

		void Create(const int &w, const int &h, const void *wind = nullptr)	{
			if (wind) {
				win_ = SDL_CreateWindowFrom(wind);
			} else {
				win_ = SDL_CreateWindow(
					"winx", SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED, w, h,
					SDL_WINDOW_SHOWN |
						SDL_WINDOW_BORDERLESS);
			}

			render_ = SDL_CreateRenderer(win_, -1, 0);
			SetOrigin(0, h);
		}
		void Run() {
			while (!quit_) {
				unique_lock<mutex> lk(mtx_);
				auto tick1 = GetTickCount64();
				HandleEvent();
				SDL_RenderClear(render_);
				Draw(dt_);
				SDL_RenderPresent(render_);
				auto tick2 = GetTickCount64();
				dt_ = tick2 - tick1;
				if (dt_ < frame_interval) {
					lk.unlock();
					SDL_Delay(frame_interval - dt_);
					dt_ = frame_interval;
				} else {
					lk.unlock();
				}
			}
		}

		virtual void Draw(const unsigned& dt) = 0;

		void DrawRect(const int &x, const int &y, const int &w, const int &h, const unsigned& color) {
			SDL_Rect rt{x, y, w, h};
			
			Uint8 red, g, b, a;
			SDL_GetRenderDrawColor(render_, &red, &g, &b, &a);
			Uint8 nr = color & 0xff;
			Uint8 ng = color >> 8 & 0xff;
			Uint8 nb = color >> 16 & 0xff;
			Uint8 na = color >> 24 & 0xff;

			SDL_SetRenderDrawColor(render_, nr, ng, nb, na);			
			SDL_RenderDrawRect(render_, &rt);
			SDL_SetRenderDrawColor(render_, red, g, b, a);
		}

		void FillRect(const int &x, const int &y, const int &w, const int &h, SDL_Texture *texture);

		void Offset(int& x, int& y, const int& w, const int& h) {
			x = x + ox_;
			y = oy_ - y - h;
		}
		void ShutDown() { quit_ = true; }
		void Destroy() {
			SDL_DestroyRenderer(render_);
			SDL_DestroyWindow(win_);
			SDL_Quit();
		}

		void OpenAudio();
	public:
		renderer* render_;
		mutex mtx_;
	protected:
		//原始Windows窗口坐标系下的点作为原点，比如0，0是左上角的点作为原点
		void SetOrigin(const int& x, const int& y)
		{
			ox_ = x;
			oy_ = y;
		}
		void HandleEvent() {
			SDL_Event se;
			SDL_PollEvent(&se);
			
			EventHandle::Instance()->OnEvent(se);
		}
	private:
		SDL_Window* win_;
		unsigned last_tick_;
		bool quit_;
		int ox_, oy_;
		unsigned dt_;
	};
	};

namespace ns_sdl_ast {
using namespace std;
template<typename T>
class Animation {
	using seq = vector<tuple<string, SDL_Texture *, int>>;
	using node = tuple<SDL_Texture *, int, int>;

public:
	Animation(renderer *r, tuple<string, SDL_Texture *, int>& first, T* parent)
		: r_(r), tick_(0),parent_(parent) {
		ts_.push_back(first);
		LoadAsset();
	}
	void Update(const unsigned &dt, const int &x, const int &y,
		    const int &w, const int &h)
	{
		tick_ += dt;
		if (tick_ >= get<1>(curr_)) {
			tick_ = 0;
			curr_ = next_;
			IsEnd();
			Next();
		}
		SDL_Rect rt = {x, y, w, h};
		SDL_Point pt = {w / 2, h / 2};
		SDL_RenderCopyEx(r_, get<0>(curr_), NULL, &rt, parent_->angle_, &pt, SDL_FLIP_NONE);
	}

	void LoadAsset()
	{
		get<0>(first_) = get<1>(ts_[0]);
		get<1>(first_) = get<2>(ts_[0]);
		get<2>(first_) = 0;

		curr_ = first_;
		Next();
	}
	seq &GetTs() {
		return ts_;
	}

	void UpdateParent(T *parent) { parent_ = parent; }

	bool IsRunEnd() { return is_run_end_; }

protected:
	void Next() {
		auto index = get<2>(curr_) + 1;		
		if (index >= ts_.size()) {
			next_ = first_;
		}
		if (index < ts_.size()) {
			get<0>(next_) = get<1>(ts_[index]);
			get<1>(next_) = get<2>(ts_[index]);
			get<2>(next_) = index;
		}
	}

	void IsEnd() {
		if (get<2>(curr_) >= ts_.size() - 1) {
			is_run_end_ = true;
		}
	}

private:
	seq ts_;
	int tick_;
	node first_;
	node curr_;
	node next_;
	renderer *r_;
	T *parent_;
	bool is_run_end_=false;
};

class AssetMgr : public single<AssetMgr> {
public:
	void SetRenderer(renderer *r) { r_ = r; }

	renderer *GetRenderer() { return r_; }
	SDL_Texture *GetTexture(const string &name)
	{
		auto it = textures_.find(name);
		if (it != textures_.end()) {
			return it->second;
		}
		return LoadTexture(name);
	}

	Mix_Music *GetMusic(const string &name, const string& fullname="") {
		auto it = musics_.find(name);
		if (it != musics_.end()) {
			return it->second;
		} else if (!fullname.empty()) {
			return LoadMuisc(name, fullname);
		}
		return nullptr;
	}

	Mix_Chunk *GetChunk(const string &name, const string &fullname = "") {
		auto it = chunks_.find(name);
		if (it != chunks_.end()) {
			return it->second;
		} else if (!fullname.empty()) {
			return LoadChunk(name, fullname);
		}
		return nullptr;
	}

	void PlayMusic(const string &name) {
		auto mus = GetMusic(name);
		if (mus) {
			Mix_PlayMusic(mus, 0);
		}
	}

	void PlayChunk(const string& name) {
		auto chk = GetChunk(name);
		if (chk) {
			Mix_PlayChannel(-1, chk, 0);
		}
	}

	bool IsPlayingMus() { return 0 != Mix_PlayingMusic();
	}

	void ReleaseAsset() {
		for (auto &it : textures_) {
			SDL_DestroyTexture(it.second);
		}
		for (auto &it : musics_) {
			Mix_FreeMusic(it.second);
		}
		for (auto &it : chunks_) {
			Mix_FreeChunk(it.second);
		}
	}

protected:
	SDL_Texture *LoadTexture(const string &name)
	{
		auto t = IMG_LoadTexture(r_, name.c_str());
		textures_.insert(make_pair(name, t));
		return t;
	}
	Mix_Music *LoadMuisc(const string &name, const string &fullname) {
		auto m = Mix_LoadMUS(fullname.c_str());
		musics_.insert(make_pair(name, m));
		return m;
	}

	Mix_Chunk *LoadChunk(const string &name, const string &fullname) {
		auto ck = Mix_LoadWAV(fullname.c_str());
		chunks_.insert(make_pair(name, ck));
		return ck;
	}

private:
	renderer *r_;
	map<string, SDL_Texture *> textures_;
	map<string, Mix_Music *> musics_;
	map<string, Mix_Chunk *> chunks_;
};
};

#endif
