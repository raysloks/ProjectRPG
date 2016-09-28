//#include "World.h"
//#include "Client.h"
//#include "Server.h"
//
//#include "GameLoop.h"
//
//#include "FieldTriangle.h"
//
////INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
////    PSTR lpCmdLine, INT nCmdShow)
//void main(void)
//{
//	World * world = new World();
//	Server * server = 0;
//	Client * client = 0;
//	world->use_chunks = false;
//	char c;
//	std::cin >> c;
//	if (c=='p')
//	{
//		client = new Client(world);
//		server = new Server(world);
//	}
//	if (c=='c')
//	{
//		client = new Client(world);
//		client->connect("127.0.0.1", 7777);
//		server = new Server(world);
//	}
//	if (c=='s')
//	{
//		server = new Server(world, 7777);
//	}
//	world->client = client;
//	world->server = server;
//	if (server!=0)
//	{
//		server->client = client;
//		if (client!=0)
//			client->server = server;
//		GameLoop gl(world, server, client);
//		gl.init();
//		if (client!=0)
//		{
//			while (client->IsAlive())
//				gl.tick();
//		}
//		else
//		{
//			while (true)
//				gl.tick();
//		}
//	}
//}