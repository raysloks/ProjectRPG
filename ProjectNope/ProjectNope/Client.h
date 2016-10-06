#ifndef CLIENT_H
#define CLIENT_H

#include "Connection.h"
#include <memory>
#include "IEventListener.h"
#include "NewEntity.h"
#include <set>
#include "Window.h"

#include "Texture.h"
#include "ShaderResource.h"

#include "Input.h"

class World;
class Server;
class ClientData;
class FrameBuffer;
class CascadedShadowMap;
class ShaderProgram;
class Shader;
class ScriptMemory;
class StandardHUD;
class RenderSetup;

class Client :
	public IEventListener
{
public:
	Client(World * pWorld);
	virtual ~Client(void);

	void setup(void);

	void connect(const std::string& address, unsigned short port);
	void disconnect(void);

	virtual bool HandleEvent(IEvent * pEvent);

	bool IsAlive(void);

	virtual void tick(float dTime);
	virtual void render(void);
//protected:

	void process_network_data(void);
	void handle_packet(const std::shared_ptr<Packet>& packet);
	void interpolate(float dTime);
	void sync(void);

	void render_world(void);

	bool isActive, isAlive, lockCursor, hideCursor;

	std::shared_ptr<Connection> con;
	boost::asio::ip::udp::endpoint endpoint;

	World * world;
	Server * server;

	std::vector<NewEntity*> interpol_targets;
	std::vector<float> interpol_elapsed;

	float interpol_delay;
	float packet_loss_sim;
	bool show_entity_list;

	bool show_back_face_lines;

	int wireframe;

	ClientData * clientData;

	Input input;

	std::vector<std::shared_ptr<Window>> windows;
	std::shared_ptr<StandardHUD> hud;

	std::vector<std::shared_ptr<std::function<void(RenderSetup&)>>> render2D;

	std::shared_ptr<FrameBuffer> buffer, frame, frame2, stencil;
	std::shared_ptr<CascadedShadowMap> shadow_buf;
	std::vector<Matrix4> dp;
	GlobalPosition prev;

	int supersample_x, supersample_y;

	std::shared_ptr<ShaderProgram> depth_fill_prog, deferred_prog, shader_program, sky_prog, dof_prog, stencil_prog;

	std::shared_ptr<ScriptMemory> mem;

	Vec3 light;
};

#endif