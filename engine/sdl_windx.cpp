#include "sdl_windx.h"

using namespace ns_sdl_ast;
namespace ns_sdl_winx {
void windowx::FillRect(const int &x, const int &y, const int &w, const int &h, SDL_Texture *texture) {
	SDL_Rect rt{x, y, w, h};

	SDL_RenderCopy(render_, texture, NULL, &rt);
}
void windowx::OpenAudio() {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 8, 2048); //…Ë÷√…˘“Ù
}

};