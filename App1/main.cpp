#include "pch.h"

#include "App.h"

int main(int argc, char** argv)
{
	auto app = std::make_unique<App>();
	if (!app->Initialize(1600, 900, "App1", true))
		return -1;
	app->GameLoop();
}
