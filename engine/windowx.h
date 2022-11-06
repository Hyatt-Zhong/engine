#include <atlbase.h>
#include <atltypes.h>
#include <Include/atlapp.h>
#include <Include/atlgdi.h>

namespace ns_windowx {
#define WM_LOOP (WM_USER + 1)
	class Loop : public CMessageLoop, public CIdleHandler {
	public:
		Loop() {}
		virtual ~Loop() { PostQuitMessage(0); }

		void Run()
		{
			Start();
		}

		void Push() { PostMessage(NULL, WM_LOOP, 0, 0); }

		virtual BOOL Init() = 0;
		virtual void UnInit() = 0;

	private:
		void Start()
		{
			AddIdleHandler(this);
			if (!Init()) {
				return;
			}
			CMessageLoop::Run();
			UnInit();
		}

	private:
	};

	//非Windows坐标系的，数学上常规的坐标系的窗口
	class windowx : public CWindowImpl<windowx, CWindow> {
	public:
		BEGIN_MSG_MAP(windowx)
		END_MSG_MAP()
		windowx() : w_(0), h_(0), ox_(0), oy_(0)
		{
		}

		~windowx()
		{
			ReleaseDC(dc_);
			DeleteDC(dc_mem_);
			DeleteObject(pen_);
			DeleteObject(bmp_);
			DeleteObject(black_);
		}

		void create(const int& w, const int& h)
		{
			w_ = w;
			h_ = h;
			CRect rt(0, 0, w_, h_);
			AdjustWindowRectEx(&rt, WS_CAPTION | WS_VISIBLE, FALSE, 0);
			Create(NULL, rt, "windowx", WS_CAPTION | WS_VISIBLE);
			CenterWindow();
			ShowWindow(SW_NORMAL);

			dc_ = GetDC();
			dc_mem_ = CreateCompatibleDC(dc_);
			bmp_ = CreateCompatibleBitmap(dc_mem_, w_, h_);
			auto old = SelectObject(dc_mem_, bmp_);
			DeleteObject(old);

			black_ = CreateSolidBrush(0);
		}

		void setcolor(COLORREF color)
		{
			pen_ = CreatePen(PS_SOLID, 1, color);

			auto old = SelectObject(dc_mem_, pen_);
			DeleteObject(old);
		}

		void moveto(const int& x, const int& y)
		{
			auto a = x, b = y;
			offset(a, b);
			MoveToEx(dc_mem_, a, b, nullptr);
		}

		void lineto(const int& x, const int& y)
		{
			auto a = x, b = y;
			offset(a, b);
			LineTo(dc_mem_, a, b);
		}

		//原始Windows窗口坐标系下的点作为原点，比如0，0是左上角的点作为原点
		void setorigin(const int& x, const int& y)
		{
			ox_ = x;
			oy_ = y;
		}

		void refresh() { BitBlt(dc_, 0, 0, w_, h_, dc_mem_, 0, 0, SRCCOPY); }

		void clear() { FillRect(dc_mem_, CRect(0, 0, w_, h_), black_); }

	private:
		void offset(int& x, int& y)
		{
			x = x + ox_;
			y = oy_ - y;
		}

	private:
		int w_, h_;
		int ox_, oy_;
		HDC dc_mem_;
		HDC dc_;
		HBITMAP bmp_;
		HPEN pen_;
		HBRUSH black_;
	};

	class MainLoop :public Loop
	{
	public:
		MainLoop()
		{
		}
		~MainLoop()
		{
		}
		BOOL Init() {
			wx_.create(500, 400);
			wx_.setorigin(0, 400);
			wx_.setcolor(0xffffff);
			return TRUE;
		}
		void UnInit() {}
		BOOL OnIdle() {
			wx_.clear();
			wx_.moveto(0, 0);
			wx_.lineto(300, 300);
			wx_.refresh();
			Sleep(20);
			Push();
			return FALSE;
		}
	protected:
	private:
		windowx wx_;
	};
}


