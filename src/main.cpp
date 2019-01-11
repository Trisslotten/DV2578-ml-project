#include "window.hpp"

#include "renderer.hpp"

int main()
{
	Window::open(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	Renderer renderer;
	renderer.init();

	while (!Window::shouldClose())
	{
		renderer.render();

		Window::update();
	}

	Window::close();
	return 0;
}