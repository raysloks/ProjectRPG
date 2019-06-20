#include "GameLoop.h"

#include "Server.h"
#include "Client.h"

#include "GameEventManager.h"
#include "TempPlatform.h"

#include <iostream>

#include <time.h>

#include "World.h"

#include "Resource.h"

#include "FontResource.h"

IEventManager * gpEventManager;

FT_Library ftLibrary;

double secondsPerStep = 1.0 / 60.0;
double fpsCap = 1.0/60.0;
float time_scale = 1.0f;
bool forceCap = true;
int forceFrameSync = -1;
bool useFrameSync = true;
bool useDynamicVSyncDisable = false;
bool useVSync = false;

#include "Script.h"
#include "BooleanVar.h"
#include "FloatVar.h"
#include "StringVar.h"

//#include "AudioSource.h"

#include <fstream>

#include "Profiler.h"

GameLoop::GameLoop(World * w, Server * s, Client * c)
{
	world = w;
	server = s;
	client = c;
}

GameLoop::~GameLoop(void)
{
	if (client)
	{
		if (ftLibrary != nullptr)
		{
			FT_Error ftError = FT_Done_FreeType(ftLibrary);
			ftLibrary = nullptr;
		}

		delete gpEventManager;
	}

	world->clear();
	world->clean();

	if (server != nullptr)
		delete server;
	if (client != nullptr)
		delete client;
	
	if (world != nullptr)
		delete world;
}

void GameLoop::init(void)
{
	if (client)
	{
		gpEventManager = new GameEventManager();

		gpEventManager->AddListener(client, KeyDownEvent::event_type);
		gpEventManager->AddListener(client, KeyUpEvent::event_type);
		gpEventManager->AddListener(client, MouseMoveEvent::event_type);
		gpEventManager->AddListener(client, MouseWheelEvent::event_type);
		gpEventManager->AddListener(client, PlatformDestroyEvent::event_type);
		gpEventManager->AddListener(client, ResizeEvent::event_type);
		//std::cout << "initializing audio..." << std::endl;
		//Sound::init();

		FT_Error ftError = FT_Init_FreeType(&ftLibrary);
		if (ftError)
		{
			MessageBoxA(0, "FreeType error initializing library!", "ERROR", MB_OK | MB_ICONERROR);
		}
	}

	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	QueryPerformanceCounter((LARGE_INTEGER*)&start_busy);

	fullInSeconds = 0.0;
	busyInSeconds = 0.0;
}

void GameLoop::tick(void)
{
	if (client)
	{
		client->pre_frame(fullInSeconds / client->subframes * time_scale);
		client->render();
		client->input.clear();
		client->platform->input(gpEventManager);
		gpEventManager->Tick();
		client->input.update();
		client->post_frame(fullInSeconds / client->subframes * time_scale);

		client->platform->set_vsync(useVSync);
		if (client->subframe == 0)
		{
			client->platform->swap();

			QueryPerformanceCounter((LARGE_INTEGER*)&end);
			fullInSeconds = static_cast<double>(end - start) / freq;
			start = end;
		}
		else
		{
			double limit = std::min(1.0 / 12.0, fullInSeconds) * (client->subframes - client->subframe) / client->subframes;
			double elapsed;
			do
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&end);
				elapsed = static_cast<double>(end - start) / freq;
			} while (elapsed < limit);
		}

		if (gRenderContext)
		{
			gRenderContext->ReserveBuffers(16);
			gRenderContext->ReserveVertexArrays(64);
		}
	}

	server->tick(secondsPerStep * time_scale);
	world->clean();

	if (!client)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&end_busy);
		busyInSeconds = static_cast<double>(end_busy - start_busy) / freq;
		Sleep(std::max(secondsPerStep - busyInSeconds, 0.0) * 1000);
		QueryPerformanceCounter((LARGE_INTEGER*)&start_busy);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);
		fullInSeconds = static_cast<double>(end - start) / freq;
		start = end;
	}

	if (client)
	{
		Profiler::set("fps", 1.0 / fullInSeconds);
		Profiler::set("sfps", client->subframes / fullInSeconds);
	}
	else
	{
		Profiler::set("server_full", 1.0 / fullInSeconds);
		Profiler::set("server_busy", 1.0 / busyInSeconds);
	}
}