#include "window.hpp"

#include "renderer.hpp"

int main()
{
	Window::open();

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