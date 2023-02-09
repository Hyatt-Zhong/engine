#include <nativelib.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <funcdecl.h>
#include <engine.h>
#include <module.h>
#include <thread>
#include <condition_variable>

using namespace ns_engine;
using namespace ns_box2d;
using namespace placeholders;
using namespace ns_module;

string strLog;
bool bClear;
void AddLog(const string& log)
{
	if (bClear)
	{
		bClear = false;
		strLog.clear();
	}
	strLog += "\n";
	strLog += log;
}

char* DllLog()
{
	bClear = true;
	return (char*)strLog.c_str();
}

BOOL CALLBACK EnumChromeWindowsProc(HWND hwnd, LPARAM lParam)
{
	AddLog("EnumChromeWindowsProc");
	char buf[256];
	if (GetClassNameA(hwnd, buf, sizeof(buf) / sizeof(*buf))) {
		AddLog(buf);
		if (strstr(buf, "Intermediate D3D Window")) {
			AddLog("Intermediate D3D Window");
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
			if ((style & WS_CLIPSIBLINGS) == 0) {
				style |= WS_CLIPSIBLINGS;
				SetWindowLongPtr(hwnd, GWL_STYLE, style);
			}
		}
	}
	return TRUE;
}
void FixChromeD3DIssue(HWND chromeWindow)
{
	// auto handle = FindWindowEx(chromeWindow, nullptr, "Intermediate D3D Window", "");
	// assert(handle != nullptr);
	AddLog("FixChromeD3DIssue");

	(void)EnumChildWindows(chromeWindow, EnumChromeWindowsProc, 0);

	LONG_PTR style = GetWindowLongPtr(chromeWindow, GWL_STYLE);
	if ((style & WS_CLIPCHILDREN) == 0) {
		style |= WS_CLIPCHILDREN;
		SetWindowLongPtr(chromeWindow, GWL_STYLE, style);
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#define WINDOW_CLASS_NAME "MyWindowClass"
void RegisterWindowClass()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC | CS_NOCLOSE | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = WINDOW_CLASS_NAME;

	RegisterClassEx(&wc);
}
HWND CreateChildWindow(int x, int y, int w, int h, HWND parent)
{
	auto VIEWPORT_WIDTH = w;
	auto VIEWPORT_HEIGHT = h;
	RECT rect;
	GetWindowRect(parent, &rect);
	int parentWidth = rect.right - rect.left;
	int parentHeight = rect.bottom - rect.top;

	auto child = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_COMPOSITED,
		WINDOW_CLASS_NAME,
		nullptr,
		WS_VISIBLE | WS_CHILD | WS_POPUP, parentWidth / 2 - VIEWPORT_WIDTH / 2,
		parentHeight / 2 - VIEWPORT_HEIGHT / 2,
		VIEWPORT_WIDTH,
		VIEWPORT_HEIGHT,
		parent,
		nullptr,
		nullptr,
		nullptr
	);
	SetParent(child, parent);
	SetLayeredWindowAttributes(child, 0, 255, LWA_ALPHA);

	return child;
}

struct data_pack {
	Game *xwx = nullptr;
	Layer xlayer;
	Scene xscene;
	int x, y, w, h;
	HWND parent;

	Actor *xmod = nullptr;

	condition_variable cv;
	mutex mtx;
	void Init(int xx, int xy, int xw, int xh, HWND hwnd) {
		parent = hwnd;
		x = xx, y = xy, w = xw, h = xh;
	}
};

data_pack *xdp = nullptr;
void* SubWindow(int x, int y, int w, int h, HWND parent)
{
	xdp = new data_pack;
	xdp->Init(x, y, w, h, parent);
	
	auto xfunc = [](data_pack *xdp) {
		auto parent = xdp->parent;
		auto x = xdp->x;
		auto y = xdp->y;
		auto w = xdp->w;
		auto h = xdp->h;

		xdp->cv.notify_all();

		RegisterWindowClass();
		FixChromeD3DIssue(parent);
		auto child = CreateChildWindow(x, y, w, h, parent);

		auto width = w;
		auto height = h;
		xdp->xwx = Game::Instance();
		auto wx = xdp->xwx;
		wx->Create(width, height, child);

		auto scene = &xdp->xscene;
		auto layer = &xdp->xlayer;

		wx->AddSub(scene);
		scene->AddSub(layer);
		scene->SetPostion(0, 0, false);
		scene->SetSize(width, height);
		layer->SetPostion(0, 0, false);
		layer->SetSize(width, height);

		MainCamera::Instance()->SetPostion(0, 0, false);
		MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
		MainWorld::Instance()->SetGravity(b2Vec2(0, 0));

		wx->Run();

		delete xdp;
	};

	auto th = new thread(xfunc, xdp);
	th->detach();

	unique_lock<mutex> lk(xdp->mtx);
	xdp->cv.wait(lk);

	return 0;
}

int TestModule(char* asstpath, char* mod) {
	if (xdp && xdp->xmod) {
		lock_guard<mutex> lk(xdp->xwx->mtx_);
		xdp->xlayer.DeleteSub(xdp->xmod);
		delete xdp->xmod;
		xdp->xmod = nullptr;
	}
	if (xdp) {
		xdp->xmod = CreateMod(xdp->xwx, asstpath, mod, &xdp->xlayer, true);
	}
	return 0;
}



