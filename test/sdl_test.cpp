#include "../engine/sdl_windx.h"
#include "../engine/engine.h"
class winx :public ns_sdl_winx::windowx,
	public Direct
{
public:
	void Draw(const unsigned& dt)
	{
		Uint8 red, g, b, a;
		SDL_GetRenderDrawColor(render_, &red, &g, &b, &a);
		SDL_SetRenderDrawColor(render_, 255, 0, 0, 255);

		//if (i % 9 < 5)
			DrawLine(0, 0, 200, 200);
		//else
			DrawLine(200, 0, 0, 200);
		i++;

		SDL_SetRenderDrawColor(render_, red, g, b, a);
	}

	void DrawLine(int m, int n, int x, int y)
	{
		Offset(m, n);
		Offset(x, y);

		SDL_RenderDrawLine(render_, m, n, x, y);
		
	}
protected:
	int i = 0;
private:
};
#undef main
void main()
{
	winx wx;
	wx.windowx::Init();
	wx.Create(500, 400);
	wx.Run();
}