#include "KrimzLib/window/window.h"


#include "KrimzLib/convert.h"
#include "KrimzLib/console.h"


// Screen
const int kl::window::screen::width = GetSystemMetrics(SM_CXSCREEN);
const int kl::window::screen::height = GetSystemMetrics(SM_CYSCREEN);

// Constructor
kl::window::window() {
	// User methods
	this->start = []() {};
	this->update = []() {};
	this->end = []() {};
	
	// Winapi variables
	this->hInstance = GetModuleHandle(nullptr);
	this->hwnd = nullptr;
	this->hdc = nullptr;
	this->hglrc = nullptr;
	this->bmpInfo = {};
	this->wndMsg = {};

	// Fullscreen data
	this->inFull = false;
	this->winStyle = NULL;
	this->winPlace = {};
}

// Destructor
kl::window::~window() {
	this->stop();
}

// Window creation
void kl::window::startNew(const kl::ivec2& size, const std::string& name, bool resizeable, bool continuous, bool opengl) {
	// Converting window name to a wstring
	std::wstring wName = kl::convert::toWString(name);

	// Registering winapi window class
	registerWindowClass(wName);

	// Creating a window
	createWindow(size, wName, resizeable);

	// Setting up bitmap info
	setupBitmapInfo();

	// Setting up OpenGL context
	if (opengl) {
		initOpenGL();
	}

	// Binding the mouse
	this->mouse.bind(hwnd);

	// Starting the update loops
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	if (continuous) {
		start();
		while (IsWindow(hwnd)) {
			while (PeekMessageW(&wndMsg, hwnd, 0, 0, PM_REMOVE)) {
				handleMessage();
			}
			update();
		}
		end();
	}
	else {
		start();
		while (IsWindow(hwnd)) {
			GetMessageW(&wndMsg, hwnd, 0, 0);
			handleMessage();
			update();
		}
		end();
	}

	// Cleanup
	cleanup(wName, opengl);
}
void kl::window::stop() const {
	PostMessageW(hwnd, WM_CLOSE, 0, 0);
}

// Returns a handle to the window
HWND kl::window::getHWND() {
	return hwnd;
}

// Sets the fullscreen mode
void kl::window::setFullscreen(bool enable) {
	if (!inFull && enable) {
		// Saving old window position
		GetWindowPlacement(hwnd, &winPlace);

		// Enabling the fullscreen
		SetWindowLong(hwnd, GWL_STYLE, winStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, kl::window::screen::width, kl::window::screen::height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		// Setting info
		inFull = true;
	}
	else if (inFull && !enable) {
		// Resetting the size
		SetWindowLong(hwnd, GWL_STYLE, winStyle);
		SetWindowPlacement(hwnd, &winPlace);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		// Setting info
		inFull = false;
	}
}

// Returns the window size
kl::ivec2 kl::window::getSize() const {
	RECT clientArea;
	GetClientRect(hwnd, &clientArea);
	return kl::ivec2(clientArea.right - clientArea.left, clientArea.bottom - clientArea.top);
}

// Returns the aspect ratio
float kl::window::getAspect() const {
	kl::ivec2 winSize = getSize();
	return (float)winSize.x / winSize.y;
}

// Returns the center point of the frame
kl::ivec2 kl::window::getCenter() const {
	return this->getSize() / 2;
}

// Sets the window title
void kl::window::setTitle(const std::string& data) {
	SetWindowTextA(hwnd, data.c_str());
}

// Sets the pixels of the window
void kl::window::drawImage(const kl::image& toDraw, const kl::ivec2& position) {
	bmpInfo.bmiHeader.biWidth = toDraw.gWidth();
	bmpInfo.bmiHeader.biHeight = toDraw.gHeight();
	StretchDIBits(hdc, position.x, (toDraw.gHeight() - 1) + position.y, toDraw.gWidth(), -toDraw.gHeight(), 0, 0, toDraw.gWidth(), toDraw.gHeight(), toDraw.pointer(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
}

// Binds the OpenGL contex of the window
void kl::window::makeCurrentGL() {
	wglMakeCurrent(hdc, hglrc);
}

// Resets the OpenGL viewport
void kl::window::resetViewport() {
	kl::gl::resetViewport(hwnd);
}

// Swaps the front and back buffers
void kl::window::swapBuffers() {
	SwapBuffers(hdc);
}

// Registers a new window class
void kl::window::registerWindowClass(const std::wstring& name) {
	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = name.c_str();
	windowClass.hIconSm = nullptr;
	kl::console::error(!RegisterClassExW(&windowClass), "WinApi: Could not register a window class!");
}

// Creates a new window
void kl::window::createWindow(const kl::ivec2& size, const std::wstring& name, bool resizeable) {
	// Setting the window properties
	winStyle = resizeable ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	RECT adjustedWindowSize = { 0, 0, (LONG)size.x, (LONG)size.y };
	AdjustWindowRect(&adjustedWindowSize, winStyle, FALSE);
	const kl::ivec2 adjSize(adjustedWindowSize.right - adjustedWindowSize.left, adjustedWindowSize.bottom - adjustedWindowSize.top);

	// Creating the window
	hwnd = CreateWindowExW(0, name.c_str(), name.c_str(), winStyle, (kl::window::screen::width / 2 - adjSize.x / 2), (kl::window::screen::height / 2 - adjSize.y / 2), adjSize.x, adjSize.y, nullptr, nullptr, hInstance, nullptr);
	kl::console::error(!hwnd, "WinApi: Could not create a window!");

	// Setting and getting window info
	ShowWindow(hwnd, SW_SHOW);
	hdc = GetDC(hwnd);

	// Saving a complete window style
	winStyle = GetWindowLong(hwnd, GWL_STYLE);
}

// Sets up the bitmap properties
void kl::window::setupBitmapInfo() {
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
}

// Sets up OpenGL context
void kl::window::initOpenGL() {
	// Init OpenGL
	hglrc = kl::gl::init(hwnd);

	// Setting the viewport size
	kl::gl::resetViewport(hwnd);
}

// Handles the windows message
void kl::window::handleMessage() {
	switch (wndMsg.message) {
	case WM_KEYDOWN:
		this->keys.setKey(wndMsg.wParam, true);
		break;

	case WM_KEYUP:
		this->keys.setKey(wndMsg.wParam, false);
		break;

	case WM_LBUTTONDOWN:
		this->mouse.lmb = true;
		break;

	case WM_LBUTTONUP:
		this->mouse.lmb = false;
		break;

	case WM_MBUTTONDOWN:
		this->mouse.mmb = true;
		break;

	case WM_MBUTTONUP:
		this->mouse.mmb = false;
		break;

	case WM_RBUTTONDOWN:
		this->mouse.rmb = true;
		break;

	case WM_RBUTTONUP:
		this->mouse.rmb = false;
		break;

	case WM_MOUSEMOVE:
		this->mouse.position = kl::ivec2(GET_X_LPARAM(wndMsg.lParam), GET_Y_LPARAM(wndMsg.lParam));
		break;

	default:
		DispatchMessageW(&wndMsg);
		break;
	}
}

// Destroys the contexts
void kl::window::cleanup(const std::wstring& name, bool opengl) {
	// Destroying the opengl context
	if (opengl) {
		kl::gl::uninit(&hglrc);
	}

	// Unregistering the window class
	UnregisterClassW(name.c_str(), hInstance);
	hdc = nullptr;
	hwnd = nullptr;
}
