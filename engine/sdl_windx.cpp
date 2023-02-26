#include "sdl_windx.h"
#include "bx2-engine.h"
#include "engine.h"
using namespace ns_box2d;
using namespace ns_sdl_ast;
using namespace ns_engine;
namespace ns_sdl_winx {
void windowx::DrawRect(const int &x, const int &y, const int &w, const int &h, const unsigned &color) {
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

void windowx::FillRect(const int &x, const int &y, const int &w, const int &h, SDL_Texture *texture) {
	SDL_Rect rt{x, y, w, h};

	SDL_RenderCopy(render_, texture, NULL, &rt);
}
void windowx::OpenAudio() {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048); //…Ë÷√…˘“Ù
}

void windowx::DrawLine(const int& x, const int& y, const int& m, const int& n) {
	auto slope = (float)(n - y) / (float)(m - x);
	auto intercept = y - slope * x;

	auto xx = m - x;
	auto yy = n - y;

	auto r = render_;
	auto fn = [&] {
		uniform_int_distribution<unsigned> u(0, 10);
		auto count = u(re()) + 5;
		vector<pair<int, int>> vv;
		{
			auto minx = xx / (count + 1);
			auto maxx = xx / (count);

			uniform_int_distribution<int> u(minx, maxx);

			for (auto idx = 0; idx < count; idx++) {
				if (idx == 0) {
					vv.push_back(make_pair(x + u(re()), 0));
				} else {
					vv.push_back(make_pair(vv[idx - 1].first + u(re()), 0));
				}
			}
		}
		uniform_int_distribution<unsigned> uu(10, 20);
		for (auto &it : vv) {
			auto xx = it.first;
			auto yy = slope * xx + intercept;
			auto dt = uu(re());
			auto miny = yy - dt;
			auto maxy = yy + dt;

			uniform_int_distribution<int> u(miny, maxy);
			it.second = u(re());
		}
		vector<pair<int, int>> vvx;
		vvx.push_back(make_pair(x, y));
		vvx.insert(vvx.end(), vv.begin(), vv.end());
		vvx.push_back(make_pair(m, n));
		count = vvx.size();
		vv = vvx;

		SDL_Point *sp = new SDL_Point[count];
		for (auto i = 0; i < count; i++) {
			sp[i].x = vv[i].first;
			sp[i].y = vv[i].second;
		}
		SDL_RenderDrawLines(r, sp, count);
		SAFE_DELETE_ARRAY(sp);
	};

	Uint8 red, g, b, a;
	SDL_GetRenderDrawColor(r, &red, &g, &b, &a);

	SDL_SetRenderDrawColor(r, 0xff, 0xff, 0x0, 0x0);
	fn();
	fn();
	SDL_SetRenderDrawColor(r, red, g, b, a);
}


};