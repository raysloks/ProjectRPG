#ifndef CLIENT_H
#define CLIENT_H

#include "Connection.h"
#include <memory>
#include "IEventListener.h"
#include "IPlatform.h"
#include "NewEntity.h"
#include <set>
#include "Window.h"
#include <random>

#include "Texture.h"

#include "Input.h"
#include "KeybindCollection.h"

class World;
class Server;
class ClientData;
class Buffer;
class FrameBuffer;
class CascadedShadowMap;
class ShaderProgram;
class Shader;
class ScriptMemory;
class StandardHUD;
class RenderSetup;
class MainMenuWindow;

class Client :
	public IEventListener
{
public:
	Client(World * pWorld);
	virtual ~Client(void);

	void setup(void);

	void connect(const std::string& address, uint16_t port);
	void disconnect(void);

	virtual bool HandleEvent(IEvent * pEvent);

	bool IsAlive(void);

	virtual void pre_frame(float dTime);
	virtual void post_frame(float dTime);
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

	IPlatform * platform;

	size_t subframe, subframes;

	std::vector<NewEntity*> interpol_targets;
	std::vector<float> interpol_elapsed;

	float interpol_delay;
	float packet_loss_sim;
	bool show_entity_list;

	float heartbeat_interval, heartbeat_elapsed;
	uint8_t heartbeat_counter;

	bool show_back_face_lines;

	int wireframe;

	std::shared_ptr<ClientData> clientData;

	Input input;

	KeybindCollection keybinds;

	std::vector<std::weak_ptr<Window>> windows;
	std::shared_ptr<StandardHUD> hud;
	std::shared_ptr<MainMenuWindow> main_menu;

	std::vector<std::shared_ptr<std::function<void(RenderSetup&)>>> render2D;

	std::shared_ptr<Buffer> depth_buf, normal_buf, light_buf, color_buf, stencil_buf, flat_stencil_buf;
	std::shared_ptr<FrameBuffer> depth_prepass_fb, normal_fb, light_fb, deferred_fb, color_fb, stencil_fb, flat_stencil_fb;

	float supersample_x, supersample_y;

	float gui_scale;

	std::shared_ptr<ShaderProgram> depth_fill_prog, shader_program, normal_prog, light_prog, sky_prog, dof_prog, stencil_prog, flat_stencil_prog, fullscreen_stencil_prog, flat_stencil_outer_prog, gui_prog;

	std::shared_ptr<ScriptMemory> mem;

	std::mt19937 random;

	Vec3 light;
	float light_size;
	std::vector<float> light_samples;
	float light_angle;

	std::vector<unsigned int> light_lookup;
	std::shared_ptr<Texture> light_lookup_tex;
};

#endif