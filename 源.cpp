#include"DirectXDemo.h"
#include"DeferredShading.h"
#include"SVODemo.h"
#include"Sponza.h"
using namespace jmxRCore;
int main()
{
	SVODemo demo(640, 480);
	demo.init();


	MSG msg = { 0 };
	Timer timer;
	timer.reset();
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			timer.tick();
			demo.update(timer.deltaTime());
			demo.run();
		}
	}

	return 0;
}