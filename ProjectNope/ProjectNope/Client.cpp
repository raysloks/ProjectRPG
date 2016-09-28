#include "streams.h"
#include "Client.h"
#include "Server.h"
#include "World.h"
#include "PlatformEvents.h"
#include "ClientEvents.h"
#include "GUIObject.h"
#include "ClientData.h"

#include "StandardHUD.h"
#include "MainMenuWindow.h"
#include "InventoryWindow.h"

#include "Resource.h"
#include "Sound.h"

#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "CascadedShadowMap.h"

#include "RenderSetup.h"

#include "Matrix4.h"
#include "Matrix3.h"

#include "Component.h"

#include "PositionComponent.h"
#include "LineComponent.h"
#include "GraphicsComponent.h"

#include "Profiler.h"

#include "Chunk.h"

class Server;
extern Server * server;

extern double secondsPerStep;
extern double fpsCap;
extern bool forceCap;
extern int forceFrameSync;
extern bool useDynamicVSyncDisable;
extern bool useVSync;

#define MAKE_PACKET std::shared_ptr<Packet> _packet_(new Packet());outstream out(&_packet_->buffer);
#define SEND_PACKET con->SendTo(_packet_, endpoint);

const unsigned int protocol = 0;

Client::Client(World * pWorld)
{
	Sound::init();

	world = pWorld;
	isAlive = true;

	lockCursor = false;
	hideCursor = false;

	show_entity_list = false;

	light = Vec3(1.0f, -1.0f, 1.0f);
	light.Normalize();

	Vec3 pole(0.0f, 0.0f, 1.0f);
	pole.Normalize();
	light *= Matrix3(0.0f, pole);

	clientData = 0;

	windows.push_back(std::shared_ptr<Window>(new MainMenuWindow(world, this, 0, 0, 0, 0)));

	interpol_delay = 2.0f/60.0f;

	packet_loss_sim = 0.0f;

	show_back_face_lines = false;
	
	wireframe = 0;

	clientData = new ClientData();
}

void Client::connect(const std::string& address, unsigned short port)
{
	endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address), port);
	con = std::shared_ptr<Connection>(new Connection(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)));

	world->authority = false;
}

void Client::disconnect(void)
{
	if (con!=0)
		con->socket_.close();
	con = 0;
	if (clientData!=0)
		delete clientData;
	clientData = 0;
	
	world->authority = true;
}

Client::~Client(void)
{
	if (con!=0)
		con->socket_.close();
	if (clientData!=0)
		delete clientData;
	Sound::release();
}

#include "StringResource.h"

#include "FloatVar.h"
#include "BooleanVar.h"

#include "IPlatform.h"

#include "Script.h"

extern IPlatform * gpPlatform;

void Client::setup(void)
{
	if (mem==0) {
		std::shared_ptr<StringResource> config = Resource::get<StringResource>("config.txt");
		if (config!=0 || Resource::is_loaded("config.txt"))
		{
			mem.reset(new ScriptMemory());
			if (config!=0)
			{
				Script script(std::istringstream(config->string));
				script.run(mem);
			}
			int screen_width = 640;
			int screen_height = 480;
			float anisotropic_filtering = 1.0f;
			bool fullscreen = false;
			bool vsync = false;
			bool force_cap = false;
			float framerate_cap = 60.0f;
			float tickrate = 60.0f;
			int force_sync = -1;
			int z_depth = 8;
			int supersampling_x = 1;
			int supersampling_y = 1;
			auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("screen_width"));
			if (var!=0)
				screen_width = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("screen_height"));
			if (var!=0)
				screen_height = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("fullscreen"));
			if (var!=0)
				fullscreen = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("vsync"));
			if (var!=0)
				vsync = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("force_cap"));
			if (var!=0)
				force_cap = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("framerate_cap"));
			if (var!=0)
				framerate_cap = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("force_sync"));
			if (var!=0) {
				force_sync = 0;
				if (var->f<0.0f)
					force_sync = -1;
				if (var->f>0.0f)
					force_sync = 1;
			}
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("tickrate"));
			if (var!=0)
				tickrate = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("anisotropic_filtering"));
			if (var!=0)
				anisotropic_filtering = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("z_depth"));
			if (var!=0)
				z_depth = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("supersampling_x"));
			if (var!=0)
				supersampling_x = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("supersampling_y"));
			if (var!=0)
				supersampling_y = var->f;
			auto bvar = std::dynamic_pointer_cast<BooleanVar>(mem->getVariable("fullscreen"));
			if (bvar!=0)
				fullscreen = bvar->b;
			bvar = std::dynamic_pointer_cast<BooleanVar>(mem->getVariable("vsync"));
			if (bvar!=0)
				vsync = bvar->b;
			bvar = std::dynamic_pointer_cast<BooleanVar>(mem->getVariable("force_cap"));
			if (bvar!=0)
				force_cap = bvar->b;

			secondsPerStep = 1.0/((double)tickrate);
			fpsCap = 1.0/((double)framerate_cap);
			forceCap = force_cap;
			forceFrameSync = force_sync;
			useVSync = vsync;

			supersample_x = supersampling_x;
			supersample_y = supersampling_y;

			gpPlatform->resize(screen_width, screen_height);
			gpPlatform->set_z_depth(z_depth);
			gpPlatform->set_fullscreen(fullscreen);
			gpPlatform->set_vsync(vsync);

			gpPlatform->apply();

			float fMaxAnisotropicFiltering = 1.0f;
			if (anisotropic_filtering>1.0f)
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fMaxAnisotropicFiltering);
			Texture::fGeneralAnisotropicFiltering = std::max(1.0f, std::min(fMaxAnisotropicFiltering, anisotropic_filtering));
		}
	}
}

void Client::tick(float dTime)
{
	isActive = true;
	ActiveEvent * ae = new ActiveEvent();
	for (std::vector<std::shared_ptr<Window>>::reverse_iterator win=windows.rbegin();win!=windows.rend();++win)
	{
		if (win->get()->handleEvent(ae)) {
			isActive = false;
			break;
		}
	}
	delete ae;

	/*if (keysPressed.find(Platform::KeyEvent::ESC)!=keysPressed.cend())
		isAlive = false;*/

	if (con==0)
	{
		if (clientData!=0)
		{
			if (clientData->unit_id==-1) {
				server->connectClientToEntity(*clientData);
			}
			if (clientData->unit_id!=-1) {
				PNEntity * unit = 0;//world->GetEntity(clientData->unit_id);
				if (unit!=0)
				{
					//world->LoadSurroundings(world->GetEntity(clientData->unit_id));

					//std::vector<int> pos; //TODO make function for this
					//pos.push_back((unit->p.x)/(chunk_size));
					//pos.push_back((unit->p.y)/(chunk_size));
					//pos.push_back((unit->p.z)/(chunk_size));

					//clientData->loading = true;
					//if (world->chunks[pos]!=0)
					//	clientData->loading = world->chunks[pos]->loading;
				}
			}
			if (!world->use_chunks)
				clientData->loading = false;
		}
	}

	/*Vec3 pole(0.0f, 1.0f, 1.0f);
	pole.Normalize();
	light *= Matrix3(dTime*M_PI/60.0f/60.0f/12.0f, pole);*/

	if (input.isPressed(Platform::KeyEvent::P))
		show_entity_list = !show_entity_list;

	if (con!=0)
	{
		if (clientData!=0)
		{
			MAKE_PACKET;
			out << (unsigned char)255;
			SEND_PACKET;
		}
		else
		{
			MAKE_PACKET;
			out << (unsigned char)0 << protocol << (unsigned char)0;
			SEND_PACKET;
		}

		process_network_data();

		interpolate(dTime);

		world->tick(dTime);

		sync();

		if (input.isDown(Platform::KeyEvent::I))
			packet_loss_sim += dTime;
		if (input.isDown(Platform::KeyEvent::K))
			packet_loss_sim -= dTime;
		if (packet_loss_sim<0.0f)
			packet_loss_sim = 0.0f;
		if (packet_loss_sim>1.0f)
			packet_loss_sim = 1.0f;
	}

	world->frame(dTime);
}

#include "Mesh.h"

#include "Sound.h"

#include "Writing.h"

#include "IPlatform.h"
extern IPlatform * gpPlatform;

void Client::render(void)
{
	setup();

	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	//if (hud==0) {
	//	hud.reset(new StandardHUD(world, this, 0, 0, 0, 0));
	//	windows.push_back(hud);
	//	//windows.push_back(std::make_shared<InventoryWindow>(world, this));
	//}

	if (Sound::isReady())
	{
		Vec3 front = Vec3(0.0f, 0.0f, -1.0f) * world->cam_rot;
		Vec3 up = Vec3(0.0f, -1.0f, 0.0f) * world->cam_rot;
		ALfloat val[6];
		val[0] = front.x;
		val[1] = front.y;
		val[2] = front.z;
		val[3] = up.x;
		val[4] = up.y;
		val[5] = up.z;
		alListenerfv(AL_ORIENTATION, val);
		Vec3 dif;
		ALfloat pos[3];
		pos[0] = dif.x;
		pos[1] = dif.y;
		pos[2] = dif.z;
		alListenerfv(AL_POSITION, pos);
	}

	if (mem!=0)
	{

		GLint view[4];

		view[0] = 0;
		view[1] = 0;
		view[2] = gpPlatform->get_width();
		view[3] = gpPlatform->get_height();

		if (frame==0) {
			std::vector<GLenum> gbuf;
			gbuf.push_back(GL_RGB16F);
			frame = std::make_shared<FrameBuffer>(view[2]*supersample_x, view[3]*supersample_y, gbuf, GL_NONE);
		} else {
			frame->resize(view[2]*supersample_x, view[3]*supersample_y);
		}

		if (frame2==0) {
			std::vector<GLenum> gbuf;
			gbuf.push_back(GL_RGB16F);
			frame2 = std::make_shared<FrameBuffer>(view[2], view[3], gbuf, GL_NONE);
		}
		else {
			frame2->resize(view[2], view[3]);
		}

		render_world();

		glClear(GL_DEPTH_BUFFER_BIT);

		QueryPerformanceCounter(&end);
		double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		Profiler::add("world-render", durationInSeconds);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(view[0], view[0]+view[2], view[1]+view[3], view[1], -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Writing::setFont("data/assets/fonts/Lora-Regular.ttf");
		Writing::setColor(0.75f, 0.75f, 0.75f, 1.0f);
		Writing::setSize(Vec2(8.0f, 16.0f));

		RenderSetup rs;

		rs.size = Vec2(view[2], view[3]);

		for (auto i=render2D.begin();i!=render2D.end();++i)
			(**i)(rs);

		if (show_entity_list)
		{
			glPushMatrix();
			glTranslatef(0.0f, 13.0f, 0.0f);
			for (auto i=world->units.begin();i!=world->units.end();++i)
			{
				if (*i!=0)
				{
					//is this long long conversion a microsoft thingy? cpp.com says there should be an int version of to string
					//Writing::render(std::to_string((long long)(i-world->units.begin()))+" "+(*i)->getName((*i)->getSerialID()));
					glTranslatef(0.0f, 16.0f, 0.0f);
				}
			}
			glPopMatrix();

			Writing::setColor(1.0f, 0.5f, 0.5f, 1.0f);

			glPushMatrix();
			glTranslatef(320.0f, 32.0f, 0.0f);
			std::vector<std::string> vec;
			std::string str = " ";
			while(str.size()>0)
			{
				str = Profiler::get();
				vec.push_back(str);
			}
			for (auto i=vec.begin();i!=vec.end();++i)
			{
				Writing::render(*i);
				glTranslatef(0.0f, 16.0f, 0.0f);
			}
			glPopMatrix();

			glPushMatrix();
			glTranslatef(16.0f, 16.0f, 0.0f);
			Writing::setColor(0.0f, 0.0f, 0.0f, 1.0f);
			Writing::render(std::to_string(world->units.size()));
			glPopMatrix();
		}

		if (packet_loss_sim>0.0f && !world->authority)
		{
			glPushMatrix();
			glTranslatef(320.0f, 16.0f, 0.0f);
			Writing::setColor(0.75f, 0.0f, 0.0f, 1.0f);
			Writing::render(std::string("WARNING: PACKET LOSS SIMULATION ")+std::to_string((long long)(packet_loss_sim*100.0f))+"%");
			glPopMatrix();
		}

		if (clientData!=0)
		{
			if (clientData->loading)
			{
				glPushMatrix();
				glTranslatef(32.0f, 64.0f, 0.0f);
				Writing::setColor(0.0f, 0.0f, 0.0f, 1.0f);
				Writing::render("Loading...");
				Writing::setColor(1.0f, 1.0f, 1.0f, 0.75f);
				Writing::render("Loading...");
				glPopMatrix();
			}
		}

		hideCursor = true;
		for (std::vector<std::shared_ptr<Window>>::iterator win=windows.begin();win!=windows.end();++win)
		{
			dynamic_cast<RectangleWindow*>(win->get())->x = view[0];
			dynamic_cast<RectangleWindow*>(win->get())->y = view[1];
			dynamic_cast<RectangleWindow*>(win->get())->w = view[2];
			dynamic_cast<RectangleWindow*>(win->get())->h = view[3];
			if (!win->get()->onRender.empty())
				win->get()->onRender();
			win->get()->render();
		}
		lockCursor = hideCursor; // allow cursor to escape window when visible

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, frame2->tex[0]);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnable(GL_FRAMEBUFFER_SRGB);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(view[0], view[1]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(view[0]+view[2], view[1]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(view[0]+view[2], view[1]+view[3]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(view[0], view[1]+view[3]);

		glEnd();

		glDisable(GL_FRAMEBUFFER_SRGB);

		QueryPerformanceCounter(&end);
		durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		Profiler::add("render", durationInSeconds);

	}
}

void Client::render_world(void)
{
	if (shader_program==0)
	{
		if (Resource::get<StringResource>("data/gfill_vert.txt")!=0 && Resource::get<StringResource>("data/gfill_frag.txt")!=0)
			shader_program = std::shared_ptr<ShaderProgram>(new ShaderProgram(
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/gfill_vert.txt")->string, GL_VERTEX_SHADER)), 
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/gfill_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}
	if (deferred_prog==0)
	{
		if (Resource::get<StringResource>("data/deferred_vert.txt")!=0 && Resource::get<StringResource>("data/deferred_frag.txt")!=0)
			deferred_prog = std::shared_ptr<ShaderProgram>(new ShaderProgram(
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/deferred_vert.txt")->string, GL_VERTEX_SHADER)), 
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/deferred_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}
	if (depth_fill_prog==0)
	{
		if (Resource::get<StringResource>("data/dfill_vert.txt")!=0 && Resource::get<StringResource>("data/dfill_frag.txt")!=0)
			depth_fill_prog = std::shared_ptr<ShaderProgram>(new ShaderProgram(
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/dfill_vert.txt")->string, GL_VERTEX_SHADER)),
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/dfill_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}
	if (sky_prog==0)
	{
		if (Resource::get<StringResource>("data/sky_vert.txt")!=0 && Resource::get<StringResource>("data/sky_frag.txt")!=0)
			sky_prog = std::shared_ptr<ShaderProgram>(new ShaderProgram(
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/sky_vert.txt")->string, GL_VERTEX_SHADER)), 
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/sky_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}
	if (dof_prog==0)
	{
		if (Resource::get<StringResource>("data/dof_vert.txt")!=0 && Resource::get<StringResource>("data/dof_frag.txt")!=0)
			dof_prog = std::shared_ptr<ShaderProgram>(new ShaderProgram(
				std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/dof_vert.txt")->string, GL_VERTEX_SHADER)),
				std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("data/dof_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}

	if (shader_program!=0 && depth_fill_prog!=0 && sky_prog!=0)
	{
		GraphicsComponent::prep();

		GLint view[4];

		view[0] = 0;
		view[1] = 0;
		view[2] = gpPlatform->get_width();
		view[3] = gpPlatform->get_height();

		Vec3 horizon = Vec3(0.2f, 0.4f, 0.8f);
		Vec3 zenith = Vec3(0.0f, 0.1f, 0.3f);

		int shadow_buffer_size = 1024;
		int splits = 4;

		float fov = 70.0f;

		float far_z = 30000.0f;
		float near_z = 1.0f;

		if (mem!=0)
		{
			auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("shadow_res"));
			if (var!=0)
				shadow_buffer_size = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("shadow_splits"));
			if (var!=0)
				splits = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("fov"));
			if (var!=0)
				fov = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("far_z"));
			if (var!=0)
				far_z = var->f;
			var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("near_z"));
			if (var!=0)
				near_z = var->f;
		}

		Vec2 fbo_size(shadow_buffer_size, shadow_buffer_size);

		if (shadow_buf==0) {
			shadow_buf = std::make_shared<CascadedShadowMap>((int)fbo_size.x, (int)fbo_size.y, splits);
		} else {
			shadow_buf->resize(fbo_size.x, fbo_size.y, splits);
		}

		if (buffer==0) {
			std::vector<GLenum> gbuf;
			gbuf.push_back(GL_RGB16F);
			gbuf.push_back(GL_RGB16F);
			buffer = std::make_shared<FrameBuffer>(view[2]*supersample_x, view[3]*supersample_y, gbuf, GL_DEPTH_COMPONENT32);
		} else {
			buffer->resize(view[2]*supersample_x, view[3]*supersample_y);
		}

		double aspect = ((double)view[2])/((double)view[3]);

		Matrix4 pers = Matrix4::Perspective(fov, aspect, near_z, far_z);

		Matrix4 rot = world->cam_rot.getConj();
		Matrix4 invrot = world->cam_rot;

		Matrix4 proj = rot * pers;

		Matrix4 biasMat;
		biasMat.mtrx[0][0] = 0.5f;
		biasMat.mtrx[1][1] = 0.5f;
		biasMat.mtrx[2][2] = 0.5f;
		biasMat.data[12] = 0.5f;
		biasMat.data[13] = 0.5f;
		biasMat.data[14] = 0.5f;

		glBindFramebuffer(GL_FRAMEBUFFER, frame->fb);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::shared_ptr<Mesh> mesh = Resource::get<Mesh>("data/assets/terrain/low.gmdl");
		std::vector<std::string> tex;
		tex.push_back("data/assets/terrain/textures/grass.tga");
		tex.push_back("data/assets/terrain/textures/RockPlate.tga");
		tex.push_back("data/assets/terrain/textures/temple.tga");

		if (mesh!=0)
		{
			for (int i=0;i<tex.size();++i)
				mesh->slotTexture(i, Resource::get<Texture>(tex[i]));
		}

		float quad_z = 1.0f/tan(fov*M_PI/360.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		glMultMatrixf((rot * Matrix4::Perspective(fov, aspect, quad_z/2.0f, quad_z*2.0f)).data);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_NORMALIZE);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glViewport(view[0], view[1], view[2]*supersample_x, view[3]*supersample_y);

		if (sky_prog!=0)
		{
			glUseProgram(sky_prog->prog);
			sky_prog->Uniform3f("horizon", horizon.x, horizon.y, horizon.z);
			sky_prog->Uniform3f("zenith", zenith.x, zenith.y, zenith.z);
			glPushMatrix();

			glMultMatrixf(rot.Inverse().data);

			glBegin(GL_QUADS);

			glVertex3f(-aspect, -1.0f, quad_z);
			glVertex3f(aspect, -1.0f, quad_z);
			glVertex3f(aspect, 1.0f, quad_z);
			glVertex3f(-aspect, 1.0f, quad_z);

			glEnd();

			glPopMatrix();
		}
		
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glViewport(0, 0, fbo_size.x, fbo_size.y);

		float light_pos[4] = {light.x, light.y, light.z, 0.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		
		dp.resize(splits);
		for (int i=0;i<splits;++i)
			dp[i] = Matrix4::Translation(world->cam_pos-prev) * dp[i];
		prev = world->cam_pos;

		for (int i=0;i<splits;++i)
		{
			LARGE_INTEGER freq, start, end;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&start);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			Vec3 lpos(light_pos[0], light_pos[1], light_pos[2]);
			lpos.Normalize();
			Matrix3 lrot(acos(lpos.Dot(Vec3(0.0f, 0.0f, 1.0f))), -lpos.Cross(Vec3(0.0f, 0.0f, 1.0f)));

			float f = pow(M_E, i);
			Vec3 frustum_pos(15.0f*f, 15.0f*f, 15.0f*f);
			Vec3 frustum_neg(-15.0f*f, -15.0f*f, -15.0f*f);
			Vec3 z = Vec3(-light_pos[0], -light_pos[1], -light_pos[2]);
			z.Normalize();

			Matrix3 invrot(lrot.Inverse());
			Vec3 x = Vec3(1.0f, 0.0f, 0.0f) * invrot;
			Vec3 y = Vec3(0.0f, 1.0f, 0.0f) * invrot;

			Vec3 dif;//Vec3(0.0f, 0.0f, 15.0f*f)*rot.Inverse()/aspect/tan(0.5f * 70.0f / 180.0f * M_PI);// = cast_origin-world->cam_pos;
			//dif /= 2.0f;

			double xl = x.Dot(Vec3(world->cam_pos+dif));
			double yl = y.Dot(Vec3(world->cam_pos+dif));
			double zl = z.Dot(Vec3(world->cam_pos+dif));
			double stepsize_x = (frustum_pos.x-frustum_neg.x)/fbo_size.x;
			double stepsize_y = (frustum_pos.y-frustum_neg.y)/fbo_size.y;
			double vsize = (frustum_pos.z-frustum_neg.z)/65536.0;
			xl /= stepsize_x;
			yl /= stepsize_y;
			zl /= vsize;
			xl -= floor(xl);
			yl -= floor(yl);
			zl -= floor(zl);
			xl *= stepsize_x;
			yl *= stepsize_y;
			zl *= vsize;

			dif -= x*xl;
			dif -= y*yl;
			//dif -= z*zl;

			//dif += z*15.0f*f;

			Matrix4 depthP;

			Matrix4 ortho;
			depthP.mtrx[0][0] = 2.0f/(frustum_pos.x-frustum_neg.x);
			depthP.mtrx[1][1] = 2.0f/(frustum_pos.y-frustum_neg.y);
			depthP.mtrx[2][2] = -2.0f/(frustum_pos.z-frustum_neg.z);
			depthP.mtrx[0][3] = -(frustum_pos.x+frustum_neg.x)/(frustum_pos.x-frustum_neg.x);
			depthP.mtrx[1][3] = -(frustum_pos.y+frustum_neg.y)/(frustum_pos.y-frustum_neg.y);
			depthP.mtrx[2][3] = (frustum_pos.z+frustum_neg.z)/(frustum_pos.z-frustum_neg.z);

			depthP *= ortho;

			Matrix4 view = Matrix4::Translation(-dif) * lrot;

			depthP = view * depthP;

			dp[i] = depthP * biasMat;

			glMultMatrixf(depthP.data);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			shadow_buf->bind(i);

			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			RenderSetup rs;

			rs.view = depthP;

			ShaderMod mod(shader_program, [depthP, view, &rs](const std::shared_ptr<ShaderProgram>& prog) {
				prog->Uniform1i("tex", 0);
				prog->UniformMatrix4fv("transform", (rs.transform*depthP).data);
			});

			rs.pushMod(mod);
			rs.pass = 0;

			world->render(rs);

			/*if (mesh!=0)
			{
				glPushMatrix();
				glTranslatef(-Vec3(world->cam_pos).x, -Vec3(world->cam_pos).y, -Vec3(world->cam_pos).z);
				glScalef(600.0f, 600.0f, 600.0f);
				mesh->render();
				glPopMatrix();
			}*/

			QueryPerformanceCounter(&end);
			double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
			Profiler::add("shadow-buffer-fill", durationInSeconds);
		}

		glCullFace(GL_BACK);

		glDepthFunc(GL_LEQUAL);

		LARGE_INTEGER freq, start, end;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);

		glBindFramebuffer(GL_FRAMEBUFFER, buffer->fb);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glViewport(view[0], view[1], view[2]*supersample_x, view[3]*supersample_y);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (show_back_face_lines) {
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_BACK, GL_LINE);
		}

		RenderSetup rs;

		rs.view = proj;

		ShaderMod mod(shader_program, [proj, &rs](const std::shared_ptr<ShaderProgram>& prog) {
			prog->Uniform1i("tex", 0);
			prog->UniformMatrix4fv("transform", (rs.transform*proj).data);
			prog->UniformMatrix4fv("normal_transform", rs.transform.data);
		});

		rs.pushMod(mod);
		rs.pass = 1;

		if (wireframe==2)
			glPolygonMode(GL_FRONT, GL_POINT);
		if (wireframe==1)
			glPolygonMode(GL_FRONT, GL_LINE);
		if (wireframe==0)
			glPolygonMode(GL_FRONT, GL_FILL);

		world->render(rs);

		glPolygonMode(GL_FRONT, GL_FILL);

		/*if (mesh!=0)
		{
			glPushMatrix();
			glTranslatef(-Vec3(world->cam_pos).x, -Vec3(world->cam_pos).y, -Vec3(world->cam_pos).z);
			glScalef(600.0f, 600.0f, 600.0f);
			mesh->render();
			glPopMatrix();
		}*/

		rs.popMod();

		if (show_back_face_lines) {
			glPolygonMode(GL_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, frame->fb);

		if (deferred_prog!=0) {
			glUseProgram(deferred_prog->prog);
			for (int i=0;i<1;++i)
			{
				glMatrixMode(GL_TEXTURE);
				for (int i=0;i<dp.size();++i)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					glLoadIdentity();
					glMultMatrixf(dp[i].data);
				}
				glActiveTexture(GL_TEXTURE0);
				glMatrixMode(GL_MODELVIEW);

				Vec3 x(1.0f, 0.0f, 0.0f);
				Vec3 y(0.0f, 1.0f, 0.0f);
				Vec3 z(0.0f, 0.0f, 1.0f);

				Matrix3 depth_to_eye = dp.back().Inverse();

				x *= depth_to_eye;
				y *= depth_to_eye;
				z *= depth_to_eye;

				deferred_prog->UniformMatrix4fv("proj", proj.Inverse().data);

				deferred_prog->Uniform1i("nSplits", splits-1);

				deferred_prog->Uniform1f("zNear", near_z);
				deferred_prog->Uniform1f("zFar", far_z);

				deferred_prog->Uniform3f("xn", x.x, x.y, x.z);
				deferred_prog->Uniform3f("yn", y.x, y.y, y.z);
				deferred_prog->Uniform3f("zn", z.x, z.y, z.z);

				deferred_prog->Uniform2f("pixel", fbo_size.x, fbo_size.y);

				Vec2 half_pixel(0.5f/fbo_size.x, 0.5f/fbo_size.y);
				deferred_prog->Uniform2f("half_pixel", half_pixel.x, half_pixel.y);

				deferred_prog->Uniform1i("nLights", 1);

				deferred_prog->Uniform1i("diffuse", 0);
				glBindTexture(GL_TEXTURE_2D, buffer->tex[0]);
				glActiveTexture(GL_TEXTURE1);
				deferred_prog->Uniform1i("normal", 1);
				glBindTexture(GL_TEXTURE_2D, buffer->tex[1]);
				glActiveTexture(GL_TEXTURE2);
				deferred_prog->Uniform1i("depth", 2);
				glBindTexture(GL_TEXTURE_2D, buffer->depth);
				glActiveTexture(GL_TEXTURE3);
				deferred_prog->Uniform1i("shadow", 3);
				glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_buf->depth);
				glActiveTexture(GL_TEXTURE0);

				deferred_prog->Uniform4f("fog_color", horizon.x, horizon.y, horizon.z, 1.0f);

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				glDisable(GL_DEPTH_TEST);

				glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(-1.0f, -1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(1.0f, -1.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(1.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(-1.0f, 1.0f);

				glEnd();

				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
				glActiveTexture(GL_TEXTURE0);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
		}

		glMatrixMode(GL_TEXTURE);
		for (int i=0;i<dp.size();++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glLoadIdentity();
		}
		glActiveTexture(GL_TEXTURE0);

		glViewport(view[0], view[1], view[2], view[3]);

		glBindFramebuffer(GL_FRAMEBUFFER, frame2->fb);
		glClear(GL_COLOR_BUFFER_BIT);

		if (dof_prog!=0) {
			glUseProgram(dof_prog->prog);
			for (int i=0;i<1;++i)
			{
				dof_prog->Uniform1f("zNear", near_z);
				dof_prog->Uniform1f("zFar", far_z);

				dof_prog->Uniform2f("pixel", 1.0f/frame2->w, 1.0f/frame2->h);

				dof_prog->Uniform1i("max_kernel_size", std::ceilf(2.0f*frame2->w/1000.0f));

				dof_prog->Uniform1f("x_size", 1.0f/supersample_x);
				dof_prog->Uniform1f("y_size", 1.0f/supersample_y);

				dof_prog->Uniform1i("diffuse", 0);
				glBindTexture(GL_TEXTURE_2D, frame->tex[0]);
				glActiveTexture(GL_TEXTURE1);
				dof_prog->Uniform1i("depth", 1);
				glBindTexture(GL_TEXTURE_2D, buffer->depth);
				glActiveTexture(GL_TEXTURE0);

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				glDisable(GL_DEPTH_TEST);

				glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(-1.0f, -1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(1.0f, -1.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(1.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(-1.0f, 1.0f);

				glEnd();

				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
		}

		glUseProgram(0);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//set matrix to camera for editor
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf(proj.data);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		rs.pass = 2;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer->depth, 0); //bind the scene depth

		if (LineComponent::mode != 0)
			glEnable(GL_DEPTH_TEST);
		LineComponent::render_all(rs);

		if (LineComponent::mode == 2)
		{
			rs.pass = 3;
			glDepthFunc(GL_NOTEQUAL);
			LineComponent::render_all(rs);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);//unbind the scene depth to allow proper deferred rendering

		glLoadIdentity();
	}
}

bool Client::HandleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==PlatformDestroyEvent::event_type) {
		PlatformDestroyEvent * ev = (PlatformDestroyEvent*)pEvent;
		isAlive = false;
		return false;
	}

	for (std::vector<std::shared_ptr<Window>>::reverse_iterator win=windows.rbegin();win!=windows.rend();++win)
	{
		if (win->get()->handleEvent(pEvent))
			return false;
	}
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		input.onDown(ev->key);
	}

	if (pEvent->GetType()==KeyUpEvent::event_type) {
		KeyUpEvent * ev = (KeyUpEvent*)pEvent;
		input.onUp(ev->key);
	}

	if (pEvent->GetType()==MouseMoveEvent::event_type) {
		MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
		if (ev->relative)
		{
			input.mouse_dif_x += ev->x;
			input.mouse_dif_y += ev->y;
		}
		else
		{
			input.mouse_x = ev->x;
			input.mouse_y = ev->y;
		}
	}
	if (pEvent->GetType()==MouseWheelEvent::event_type) {
		MouseWheelEvent * ev = (MouseWheelEvent*)pEvent;
		input.mouse_dif_z += ev->d;
	}

	if (pEvent->GetType()==ResizeEvent::event_type) {
		ResizeEvent * ev = (ResizeEvent*)pEvent;
	}
	return false;
}

bool Client::IsAlive(void)
{
	return isAlive;
}

/*void Client::updateControlState(void)
{
	Vec3 f = input.ctrl[0].left_analog.out;

	if (isActive)
	{
		if (input.isDown(Platform::KeyEvent::W))
			f.y += 4.0f;
		if (input.isDown(Platform::KeyEvent::A))
			f.x -= 4.0f;
		if (input.isDown(Platform::KeyEvent::S))
			f.y -= 4.0f;
		if (input.isDown(Platform::KeyEvent::D))
			f.x += 4.0f;
	}

	Vec2 local_dir = f;
	f.x = local_dir.x*cos(camera.x)-local_dir.y*sin(camera.x);
	f.y = local_dir.x*sin(camera.x)+local_dir.y*cos(camera.x);

	cs.input["x"] = f.x;
	cs.input["y"] = f.y;
	cs.input["z"] = f.z;

	cs.input["dir_x"] = camera.x;
	cs.input["dir_y"] = camera.y;

	if (isActive)
	{
		if (input.isPressed(Platform::KeyEvent::SPACE))
			cs.activate("jump");

		if (input.isPressed(Platform::KeyEvent::LMB))
			cs.activate("attack");
		
		if (input.isPressed(Platform::KeyEvent::RMB))
			cs.activate("block");
	}

	//cs.input["lock"] = keysDown.find(Platform::KeyEvent::RMB)!=keysDown.cend() && isActive ? 1.0f : 0.0f;

	cs.input["shift"] = input.isDown(Platform::KeyEvent::LSHIFT) && isActive ? 1.0f : 0.0f;
	cs.input["shift"] += input.ctrl[0].right_trigger.out;

	cs.input["emote"] = input.isDown(Platform::KeyEvent::V) && isActive ? 1.0f : 0.0f;

	cs.input["jump"] = input.isDown(Platform::KeyEvent::SPACE) && isActive ? 1.0f : 0.0f;
	cs.input["jump"] += input.ctrl[0].left_trigger.out;
	cs.input["jump"] += input.ctrl[0].a.down;
}*/

void Client::process_network_data(void)
{
	con->mutex.lock();
	std::queue<std::shared_ptr<Packet>> recv_buf(con->recv_buf);
	while(con->recv_buf.size())
		con->recv_buf.pop();
	con->mutex.unlock();

	while(recv_buf.size())
	{
		handle_packet(recv_buf.front());
		recv_buf.pop();
	}
}

void Client::handle_packet(const std::shared_ptr<Packet>& packet)
{
	if (((float)rand())/RAND_MAX<packet_loss_sim) // simulate packet loss
		return;

	instream in(&packet->buffer);
	unsigned char type;
	in >> type;
	//try
	{
		switch(type) {
		case 0:
			{
				/*if (clientData==0)
					clientData = new ClientData(in);
				else
					clientData->read(in);*/
				break;
			}
		case 1: // notify of entity
			{
				int id, uid;
				in >> id >> uid;
				NewEntity * current_entity = world->GetEntity(id);
				if (current_entity!=0)
					if (!SyncState::is_ordered(world->uid[id], uid))
						break;
				NewEntity * unit = new NewEntity(in, false);
				world->SetEntity(id, unit);
				NewEntity * copy = new NewEntity();
				for (auto i=unit->components.begin();i!=unit->components.end();++i) {
					if (*i!=0) {
						copy->components.push_back(dynamic_cast<Component*>(Serializable::getFactory((*i)->getSerialID())->create(*i)));
						(*i)->connect(unit, false);
						(*i)->entity = unit;
						copy->components.back()->connect(copy, false);
						copy->components.back()->entity = copy;
					} else
						copy->components.push_back(0);
				}
				copy->component_sync = unit->component_sync;
				if (id >= interpol_targets.size()) {
					interpol_targets.resize(id+1);
					interpol_elapsed.resize(id+1);
				}
				if (id >= clientData->per_entity_sync.size()) {
					clientData->per_entity_sync.resize(id+1);
				}

				if (interpol_targets[id]!=0)
					delete interpol_targets[id];
				interpol_targets[id] = copy;
				interpol_elapsed[id] = 0.0f;
				break;
			}
		case 2: // entity log
			{
				int id, uid, sync;
				in >> id >> uid >> sync;
				if (id < interpol_targets.size())
				{
					auto ent = world->GetEntity(id);
					if (ent!=0) {
						if (SyncState::is_ordered(world->uid[id], uid)) {
							world->SetEntity(id, 0);
							if (id < interpol_targets.size()) {
								if (interpol_targets[id]!=0)
									delete interpol_targets[id];
								interpol_targets[id] = 0;
							}
							break; //TODO cache log until notification of entity creation is received
						}
						if (SyncState::is_ordered(clientData->per_entity_sync[id], sync))
						{
							clientData->per_entity_sync[id] = sync;
							int sync_val;
							uint32_t sync_len, sync_index;
							in >> sync_len;
							std::map<size_t, int> to_confirm;
							for (size_t i=0;i<sync_len;++i)
							{
								in >> sync_index >> sync_val;
								ent->ss.set(sync_index, sync_val);
								to_confirm.insert(std::make_pair(sync_index, sync_val));
							}
							if (!to_confirm.empty())
							{
								MAKE_PACKET

								out << (unsigned char)1 << id << (uint32_t)to_confirm.size();
								for (auto i=to_confirm.begin();i!=to_confirm.end();++i)
									out << (uint32_t)i->first << i->second;

								SEND_PACKET
							}

							if (interpol_targets[id]!=0)
								interpol_targets[id]->readLog(in);
							interpol_elapsed[id] = 0.0f;
						}
					}
				}
				break;
			}
		case 3: // delete entity
			{
				int id, uid;
				in >> id >> uid;
				if (id>=0)
				{
					if (id<world->units.size())
					{
						if (uid==world->uid[id])
						{
							world->SetEntity(id, 0);
							if (id < interpol_targets.size()) {
								if (interpol_targets[id]!=0)
									delete interpol_targets[id];
								interpol_targets[id] = 0;
							}
						}
					}
				}
				break;
			}
		default:
			{
			}
		}
		//std::cout << "packet, type " << (int)type << std::endl;
	}
	/*catch(std::exception& e)
	{
		std::cout << "protocol exception " << e.what() << " in branch " << (int)type << std::endl;
	}*/
}

void Client::interpolate(float dTime)
{
	for (int i=0;i<world->units.size();++i)
	{
		NewEntity * ent = world->GetEntity(i);
		if (ent!=0) {
			if (i<interpol_targets.size()) {
				if (interpol_targets[i]!=0) {
					if (interpol_elapsed[i]>=interpol_delay) {
						ent->interpolate(interpol_targets[i], 1.0f);
					} else {
						ent->interpolate(interpol_targets[i], std::min(1.0f, std::max(0.0f, dTime/(interpol_delay-interpol_elapsed[i]))));
						interpol_elapsed[i] += dTime;
					}
				}
			}
		}
	}
}

void Client::sync(void)
{
	if (clientData!=0)
	{
		std::set<int> missing;
		for (int i=0;i<world->units.size();++i)
		{
			NewEntity * ent = world->GetEntity(i);
			if (ent==0)
			{
				if (clientData->known_units[i]>=0)
				{
					missing.insert(i); // wtf? this need fixing // what needs fixing? dammit past me!
				}
			}
			else
			{
				std::stringbuf cbuf;
				outstream comp(&cbuf);
				ent->writeLog(comp);
				if (cbuf.str().size())
				{
					MAKE_PACKET

					out << (unsigned char)7 << i;
					out.write(cbuf.str().data(), cbuf.str().size());

					SEND_PACKET
				}
			}
		}
		if (missing.size()>0)
		{
			MAKE_PACKET

			out << (unsigned char)2;

			out << (uint32_t)missing.size();
			for (auto i=missing.begin();i!=missing.end();++i)
			{
				out << *i;
			}

			SEND_PACKET
		}
	}
}