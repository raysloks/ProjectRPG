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
IPlatform * gpPlatform;

FT_Library ftLibrary;

double secondsPerStep = 1.0/60.0;
double fpsCap = 1.0/60.0;
bool forceCap = true;
int forceFrameSync = -1;
bool useFrameSync = true;
bool useDynamicVSyncDisable = false;
bool useVSync = false;

LARGE_INTEGER freq, start, end;

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
	world->clear(false);

	if (server != nullptr)
		delete server;
	if (client != nullptr)
		delete client;
	
	if (world != nullptr)
		delete world;

	Resource::unload();

	if (ftLibrary != nullptr) {
		FT_Error ftError = FT_Done_FreeType(ftLibrary);
	}

	gpPlatform->release();

	delete gpPlatform;
	delete gpEventManager;
}

void GameLoop::init(void)
{
	gpPlatform = new TempPlatform();
	gpPlatform->set_position(4, 24);
	gpPlatform->set_z_depth(8);
	gpEventManager = new GameEventManager();

	FT_Error ftError = FT_Init_FreeType(&ftLibrary);
	if (ftError)
	{
		MessageBoxA(0, "FreeType error initializing library!", "ERROR", MB_OK | MB_ICONERROR);
	}

	if (client != nullptr)
	{
		gpEventManager->AddListener(client, KeyDownEvent::event_type);
		gpEventManager->AddListener(client, KeyUpEvent::event_type);
		gpEventManager->AddListener(client, MouseMoveEvent::event_type);
		gpEventManager->AddListener(client, MouseWheelEvent::event_type);
		gpEventManager->AddListener(client, PlatformDestroyEvent::event_type);
		gpEventManager->AddListener(client, ResizeEvent::event_type);
		//std::cout << "initializing audio..." << std::endl;
		//Sound::init();
	}

	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	durationInSeconds = fpsCap;
	fpsCorrection = 0.0;
	lag = 0.0;
}

void GameLoop::tick(void)
{
	if (client != nullptr) {
		client->pre_frame(durationInSeconds);
		client->render();
		client->input.clear();
		gpPlatform->input(gpEventManager, client->lockCursor, client->hideCursor);
		gpEventManager->Tick();
		client->input.update();
		client->post_frame(durationInSeconds);
	}

	useFrameSync = fpsCap==secondsPerStep && secondsPerStep!=0.0;// && !gpPlatform->get_vsync(); //vsync can still desync with tickrate
	if (forceFrameSync<0)
		useFrameSync = false;
	if (forceFrameSync>0)
		useFrameSync = true;

	if (useFrameSync) {
		QueryPerformanceCounter(&end);
		durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		/*if (lag+durationInSeconds<0.0)
			Sleep(-(lag+durationInSeconds)*1000);*/
		/*while (lag+durationInSeconds<0.0) {
			QueryPerformanceCounter(&end);
			durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		}*/
		server->tick(secondsPerStep);
		lag -= secondsPerStep;
	} else {
		if (lag>secondsPerStep*0.5) { // TODO fix
			if (world->authority)
				server->tick(secondsPerStep);
			/*if (client != 0)
				client->tick(lag);*/
			lag -= lag;
		}
	}
	world->clean();

	//TODO averaged inputs when fps > tick rate

	gpPlatform->set_vsync(useVSync);

	QueryPerformanceCounter(&end);
	durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	if ((!useVSync || forceCap) && fpsCap>0.0) // capping fps cap to minimum 20/30/60 something might be a good idea
	{
		Sleep(std::max(fpsCap - durationInSeconds - fpsCorrection, 0.0)*1000.0);
		QueryPerformanceCounter(&end);
		durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		fpsCorrection += durationInSeconds - fpsCap;
		if (fpsCorrection>fpsCap)
			fpsCorrection = fpsCap;
	}
	lag += durationInSeconds;
	QueryPerformanceCounter(&start);

	Profiler::set("fps", 1.0 / durationInSeconds);

	Profiler::start("swap");

	gpPlatform->swap();

	Profiler::start("synced_operations");
	if (gRenderContext)
	{
		gRenderContext->ReserveBuffers(16);
		gRenderContext->ReserveVertexArrays(64);
	}
	Profiler::stop();

	Profiler::stop();
}