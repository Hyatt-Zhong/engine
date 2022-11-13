#ifndef SDL_WINDX_H
#define SDL_WINDX_H
#include <windows.h>
#include <SDL/include/SDL.h>
#include <SDL/include/SDL_image.h>
#include <common.h>

using renderer = SDL_Renderer;
namespace ns_sdl_winx {
	using namespace std;
	const unsigned frame_interval = 13;
	const unsigned key_count = 32;

	/*using key_handle = void(*)();
	using mouse_handle = void(*)(const int& x, const int& y);
	using event_handle = void(*)();*/
	using key_handle = function<void()>;
	using mouse_handle = function<void(const int& x, const int& y, const Uint8& button)>;
	using event_handle = function<void(const SDL_Event& e)>;
	class EventHandle :public single<EventHandle>
	{
	public:
		EventHandle() {
			key_state_ = SDL_GetKeyboardState(NULL);
		}
		void AddKeyUpHandle(SDL_Keycode key, key_handle hdl) {
			if (key_up_handle_.find(key) == key_up_handle_.end()) {
				key_up_handle_.insert(make_pair(key, hdl));
			}
		}
		void AddKeyDownHandle(SDL_Keycode key, key_handle hdl) {
			if (key_down_handle_.find(key) == key_down_handle_.end()) {
				key_down_handle_.insert(make_pair(key, hdl));
			}
		}
		void AddMouseHandle(Uint32 key, mouse_handle hdl) {
			if (mouse_handle_.find(key) == mouse_handle_.end()) {
				mouse_handle_.insert(make_pair(key, hdl));
			}
		}
		void AddEventHandle(Uint32 key, event_handle hdl) {
			if (event_handle_.find(key) == event_handle_.end()) {
				event_handle_.insert(make_pair(key, hdl));
			}
		}
		void SetMouseLUpHandle(mouse_handle hdl) {
			mouse_left_up_handle_ = hdl;
		}

		bool GetKeyState(const SDL_Scancode &key){
			return key_state_[key];
		}

		//获取鼠标按钮是否按下状态
		//mouse index 1:left 2:middle 3:right
		bool GetMouseState(const int &index, int *x = nullptr, int *y = nullptr) {
			return SDL_GetMouseState(x, y) & SDL_BUTTON(index);
		}

		/*bool MonitorKey(const SDL_Keycode& key) {
			if (key_monitor_.key_pos_.size() >= key_count) {
				return false;
			}
			if (key_monitor_.key_pos_.find(key) !=
			    key_monitor_.key_pos_.end()) {
				return false;
			}
			key_monitor_.key_pos_.insert(
				make_pair(key, key_monitor_.key_count_));
			key_monitor_.key_count_++;
			return true;
		}
		bool GetKeyState(const SDL_Keycode& key) {
			auto it = key_monitor_.key_pos_.find(key);
			if (it == key_monitor_.key_pos_.end()) {
				return false;
			}
			auto pos = it->second;
			return key_monitor_.key_state_[pos];
		}*/
		void OnEvent(const SDL_Event& e) {
			switch (e.type) {
			case SDL_KEYUP:
			{
				auto it = key_up_handle_.find(e.key.keysym.sym);
				if (it != key_up_handle_.end()) {
					it->second();
				}
			}
			break;
			case SDL_KEYDOWN:
			{
				auto it = key_down_handle_.find(e.key.keysym.sym);
				if (it != key_down_handle_.end()) {
					it->second();
				}
			}
			break;
			case SDL_MOUSEBUTTONUP:
			{
				if (mouse_left_up_handle_) {
					mouse_left_up_handle_(e.button.x, e.button.y, e.button.button);
				}
			}
			break;
			default:
			{
				auto itor = mouse_handle_.find(e.type);
				if (itor != mouse_handle_.end()) {
					itor->second(e.button.x, e.button.y, e.button.button);
					break;
				}
				auto it = event_handle_.find(e.type);
				if (it != event_handle_.end()) {
					it->second(e);
					break;
				}
			}
			break;
			}
		}
	protected:
	private:
		map<SDL_Keycode, key_handle> key_up_handle_;
		map<SDL_Keycode, key_handle> key_down_handle_;
		map<Uint32, mouse_handle> mouse_handle_;
		map<Uint32, event_handle> event_handle_;
		mouse_handle mouse_left_up_handle_;

		const Uint8 *key_state_ = nullptr;
		/*struct KeyMonitor {
			bitset<key_count> key_state_;
			map<SDL_Keycode, int> key_pos_;
			int key_count_ = 0;
		};
		KeyMonitor key_monitor_;*/
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
				auto tick1 = GetTickCount64();
				HandleEvent();
				SDL_RenderClear(render_);
				Draw(dt_);
				SDL_RenderPresent(render_);
				auto tick2 = GetTickCount64();
				dt_ = tick2 - tick1;
				if (dt_ < frame_interval) {
					SDL_Delay(frame_interval - dt_);
					dt_ = frame_interval;
				}
			}
		}

		virtual void Draw(const unsigned& dt) = 0;

		void Offset(int& x, int& y, const int& w, const int& h) {
			x = x + ox_;
			y = oy_ - y - h;
		}
		void Destroy() {
			SDL_DestroyRenderer(render_);
			SDL_DestroyWindow(win_);
			SDL_Quit();
		}
	public:
		renderer* render_;
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

namespace ns_sdl_img {
using namespace std;
class Animation {
	using seq = vector<tuple<string, SDL_Texture *, int>>;
	using node = tuple<SDL_Texture *, int, int>;

public:
	Animation(renderer *r) : r_(r), tick_(0) {}
	void Update(const unsigned &dt, const int &x, const int &y,
		    const int &w, const int &h)
	{
		tick_ += dt;
		if (tick_ >= get<1>(curr_)) {
			tick_ = 0;
			curr_ = next_;
			Next();
		}
		SDL_Rect rt = {x, y, w, h};
		SDL_RenderCopy(r_, get<0>(curr_), NULL, &rt);
	}

	void LoadAsset()
	{
		get<0>(first_) = get<1>(ts_[0]);
		get<1>(first_) = get<2>(ts_[0]);
		get<2>(first_) = 0;

		curr_ = first_;
		Next();
	}
	seq ts_;

protected:
	void Next()
	{
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

private:
	int tick_;
	node first_;
	node curr_;
	node next_;
	renderer *r_;
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

	void ReleaseTexture()
	{
		for (auto &it : textures_) {
			SDL_DestroyTexture(it.second);
		}
	}

protected:
	SDL_Texture *LoadTexture(const string &name)
	{
		auto t = IMG_LoadTexture(r_, name.c_str());
		textures_.insert(make_pair(name, t));
		return t;
	}

private:
	renderer *r_;
	map<string, SDL_Texture *> textures_;
};
};

#endif
