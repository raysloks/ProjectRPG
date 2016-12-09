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
#include "Buffer.h"
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
	light_size = 0.003f;

	clientData = 0;

	windows.push_back(std::shared_ptr<Window>(new MainMenuWindow(world, this, 0, 0, 0, 0)));

	interpol_delay = 2.0f/60.0f;

	packet_loss_sim = 0.0f;

	show_back_face_lines = false;
	
	wireframe = 0;

	clientData = new ClientData();

	depth_fill_prog = std::make_shared<ShaderProgram>("data/dfill_vert.txt", "data/dfill_frag.txt");
	shader_program = std::make_shared<ShaderProgram>("data/gfill_vert.txt", "data/gfill_frag.txt");
	deferred_prog = std::make_shared<ShaderProgram>("data/deferred_vert.txt", "data/deferred_frag.txt");
	sky_prog = std::make_shared<ShaderProgram>("data/fullscreen_vert.txt", "data/sky_frag.txt");
	dof_prog = std::make_shared<ShaderProgram>("data/fullscreen_vert.txt", "data/dof_frag.txt");
	stencil_prog = std::make_shared<ShaderProgram>("data/stencil_vert.txt", "data/stencil_geom.txt", "data/stencil_frag.txt");
	flat_stencil_prog = std::make_shared<ShaderProgram>("data/flat_stencil_vert.txt", "data/flat_stencil_geom.txt", "data/flat_stencil_frag.txt");
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

	Vec3 pole(0.0f, 0.0f, 1.0f);
	pole.Normalize();
	float lspeed = M_PI / 60.0f / 60.0f / 12.0f*5000.0f;
	if (input.isDown(Platform::KeyEvent::L))
		light *= Matrix3(dTime * lspeed, pole);
	if (input.isDown(Platform::KeyEvent::J))
		light *= Matrix3(-dTime * lspeed, pole);
	if (input.isDown(Platform::KeyEvent::O))
		light_size += dTime / 50.0f;
	if (input.isDown(Platform::KeyEvent::I))
		light_size -= dTime / 50.0f;
	if (light_size < 0.0f)
		light_size = 0.0f;

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

	if (mem != 0)
	{

		GLint view[4];

		view[0] = 0;
		view[1] = 0;
		view[2] = gpPlatform->get_width();
		view[3] = gpPlatform->get_height();

		/*if (frame==0) {
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
		}*/

		render_world();

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//QueryPerformanceCounter(&end);
		//double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		//Profiler::add("world-render", durationInSeconds);

		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_LIGHTING);
		//glDisable(GL_CULL_FACE);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glOrtho(view[0], view[0]+view[2], view[1]+view[3], view[1], -1.0f, 1.0f);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();

		//Writing::setFont("data/assets/fonts/Lora-Regular.ttf");
		//Writing::setColor(0.75f, 0.75f, 0.75f, 1.0f);
		//Writing::setSize(Vec2(8.0f, 16.0f));

		//RenderSetup rs;

		//rs.size = Vec2(view[2], view[3]);

		//for (auto i=render2D.begin();i!=render2D.end();++i)
		//	(**i)(rs);

		//if (show_entity_list)
		//{
		//	glPushMatrix();
		//	glTranslatef(0.0f, 13.0f, 0.0f);
		//	for (auto i=world->units.begin();i!=world->units.end();++i)
		//	{
		//		if (*i!=0)
		//		{
		//			//is this long long conversion a microsoft thingy? cpp.com says there should be an int version of to string
		//			//Writing::render(std::to_string((long long)(i-world->units.begin()))+" "+(*i)->getName((*i)->getSerialID()));
		//			glTranslatef(0.0f, 16.0f, 0.0f);
		//		}
		//	}
		//	glPopMatrix();

		//	Writing::setColor(1.0f, 0.5f, 0.5f, 1.0f);

		//	glPushMatrix();
		//	glTranslatef(320.0f, 32.0f, 0.0f);
		//	std::vector<std::string> vec;
		//	std::string str = " ";
		//	while(str.size()>0)
		//	{
		//		str = Profiler::get();
		//		vec.push_back(str);
		//	}
		//	for (auto i=vec.begin();i!=vec.end();++i)
		//	{
		//		Writing::render(*i);
		//		glTranslatef(0.0f, 16.0f, 0.0f);
		//	}
		//	glPopMatrix();

		//	glPushMatrix();
		//	glTranslatef(16.0f, 16.0f, 0.0f);
		//	Writing::setColor(0.0f, 0.0f, 0.0f, 1.0f);
		//	Writing::render(std::to_string(world->units.size()));
		//	glPopMatrix();
		//}

		//if (packet_loss_sim>0.0f && !world->authority)
		//{
		//	glPushMatrix();
		//	glTranslatef(320.0f, 16.0f, 0.0f);
		//	Writing::setColor(0.75f, 0.0f, 0.0f, 1.0f);
		//	Writing::render(std::string("WARNING: PACKET LOSS SIMULATION ")+std::to_string((long long)(packet_loss_sim*100.0f))+"%");
		//	glPopMatrix();
		//}

		//if (clientData!=0)
		//{
		//	if (clientData->loading)
		//	{
		//		glPushMatrix();
		//		glTranslatef(32.0f, 64.0f, 0.0f);
		//		Writing::setColor(0.0f, 0.0f, 0.0f, 1.0f);
		//		Writing::render("Loading...");
		//		Writing::setColor(1.0f, 1.0f, 1.0f, 0.75f);
		//		Writing::render("Loading...");
		//		glPopMatrix();
		//	}
		//}

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

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, color_buf->gl_texture_id);

		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//glEnable(GL_FRAMEBUFFER_SRGB);

		//glBegin(GL_QUADS);

		//glTexCoord2f(0.0f, 1.0f);
		//glVertex2i(view[0], view[1]);
		//glTexCoord2f(1.0f, 1.0f);
		//glVertex2i(view[0]+view[2], view[1]);
		//glTexCoord2f(1.0f, 0.0f);
		//glVertex2i(view[0]+view[2], view[1]+view[3]);
		//glTexCoord2f(0.0f, 0.0f);
		//glVertex2i(view[0], view[1]+view[3]);

		//glEnd();

		//glDisable(GL_FRAMEBUFFER_SRGB);

		//QueryPerformanceCounter(&end);
		//durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		//Profiler::add("render", durationInSeconds);

		/*{
			GLenum err = GL_NO_ERROR;
			while ((err = glGetError()) != GL_NO_ERROR)
				std::cout << "OpenGL error at end of frame: 0x" << (void*)err << std::endl;
		}*/
	}
}

#include "PoissonGenerator.h"

void Client::render_world(void)
{
	GraphicsComponent::prep();

	int view_w = gpPlatform->get_width();
	int view_h = gpPlatform->get_height();

	int buffer_w = view_w * supersample_x;
	int buffer_h = view_h * supersample_y;

	Vec3 horizon = Vec3(0.2f, 0.4f, 0.8f);
	Vec3 zenith = Vec3(0.0f, 0.1f, 0.3f);

	float fov = 70.0f;

	float far_z = 30000.0f;
	float near_z = 1.0f;

	if (mem!=0)
	{
		auto var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("fov"));
		if (var!=0)
			fov = var->f;
		var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("far_z"));
		if (var!=0)
			far_z = var->f;
		var = std::dynamic_pointer_cast<FloatVar>(mem->getVariable("near_z"));
		if (var!=0)
			near_z = var->f;
	}


	// initialize and maintain buffers
	if (depth_buf == nullptr)
		depth_buf = std::make_shared<Buffer>();
	depth_buf->setFormat(BUFFER_DEPTH, 32);
	depth_buf->resize(buffer_w, buffer_h);

	if (normal_buf == nullptr)
		normal_buf = std::make_shared<Buffer>();
	normal_buf->setFormat(BUFFER_SIGNED_NORMALIZED, 16, 16, 16);
	normal_buf->resize(buffer_w, buffer_h);

	if (color_buf == nullptr)
		color_buf = std::make_shared<Buffer>();
	color_buf->setFormat(BUFFER_FLOAT, 32, 32, 32);
	color_buf->resize(buffer_w, buffer_h);

	if (depth_prepass_fb == nullptr)
		depth_prepass_fb = std::make_shared<FrameBuffer>();
	depth_prepass_fb->depth = depth_buf;

	if (deferred_fb == nullptr)
		deferred_fb = std::make_shared<FrameBuffer>();
	deferred_fb->color.resize(1);
	deferred_fb->color[0] = color_buf;
	deferred_fb->depth = depth_buf;

	if (color_fb == nullptr)
		color_fb = std::make_shared<FrameBuffer>();
	color_fb->color.resize(1);
	color_fb->color[0] = color_buf;
	

	// calculate projection matrix
	double aspect = ((double)view_w)/((double)view_h);

	Matrix4 pers = Matrix4::Perspective(fov, aspect, near_z, far_z);

	std::uniform_real_distribution<float> jitter_distribution(-0.5f, 0.5f);
	Vec2 view_texel_jitter(jitter_distribution(random), jitter_distribution(random));
	pers *= Matrix4::Translation(Vec3(view_texel_jitter.x / buffer_w, view_texel_jitter.y / buffer_h, 0.0f));

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


	// render background
	if (sky_prog->Use())
	{
		color_fb->bind();
		glViewport(0, 0, buffer_w, buffer_h);

		sky_prog->Uniform("horizon", horizon.x, horizon.y, horizon.z);
		sky_prog->Uniform("zenith", zenith.x, zenith.y, zenith.z);
		sky_prog->Uniform("light", light);
		sky_prog->UniformMatrix4fv("proj_inv", proj.Inverse().data);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	

	// render depth pre-pass
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		depth_prepass_fb->bind();
		glViewport(0, 0, buffer_w, buffer_h);
		glClear(GL_DEPTH_BUFFER_BIT);

		RenderSetup rs;
		rs.view = proj;

		ShaderMod mod(depth_fill_prog, [proj, &rs](const std::shared_ptr<ShaderProgram>& prog) {
			prog->UniformMatrix4fv("transform", (rs.transform*proj).data);
		});

		rs.pushMod(mod);
		rs.pass = 1;

		world->render(rs);

		rs.popMod();
	}


	// raytracing shadows setup
	{
		// calculate noise shadow sample distribution
		if (input.isDown(Platform::KeyEvent::N))
		{
			light_samples.resize(128 * 3);
			light_samples[0] = light.x;
			light_samples[1] = light.y;
			light_samples[2] = light.z;
			std::uniform_real_distribution<float> uni_dist;
			Vec3 x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
			Vec3 y = light.Cross(x).Normalize();
			for (int i = 3; i < 128 * 3; i += 3)
			{
				float t = 2.0f * M_PI * uni_dist(random);
				float r = sqrt(uni_dist(random));
				Vec3 l = (light + (x * cos(t) + y * sin(t)) * r * light_size).Normalize();
				light_samples[i] = l.x;
				light_samples[i + 1] = l.y;
				light_samples[i + 2] = l.z;
			}
		}

		// calculate poisson shadow sample distribution
		if (input.isDown(Platform::KeyEvent::M))
		{
			std::uniform_int_distribution<uint32_t> seeder;
			auto points = PoissonGenerator::GeneratePoissonPoints(128 * 2, PoissonGenerator::DefaultPRNG(seeder(random)));
			light_samples.resize(128 * 3);
			light_samples[0] = light.x;
			light_samples[1] = light.y;
			light_samples[2] = light.z;
			Vec3 x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
			Vec3 y = light.Cross(x).Normalize();
			for (int i = 1; i < 128; ++i)
			{
				Vec3 l = (light + (x * (points[i].x * 2.0f - 1.0f) + y * (points[i].y * 2.0f - 1.0f)) * light_size).Normalize();
				light_samples[i * 3] = l.x;
				light_samples[i * 3 + 1] = l.y;
				light_samples[i * 3 + 2] = l.z;
			}
		}

		// calculate hexagon grid shadow sample distribution
		if (input.isDown(Platform::KeyEvent::H))
		{
			std::vector<Vec2> points;
			std::set<std::pair<int, int>> visited;
			std::multimap<float, std::pair<int, int>> open;
			open.insert(std::make_pair(0.0f, std::make_pair(0, 0)));
			std::uniform_int_distribution<uint32_t> exangle_dist(0U, 6U);
			light_angle += 1.0f + exangle_dist(random);
			Vec2 hex_x(cos(light_angle), sin(light_angle));
			Vec2 hex_y(cos(light_angle + M_PI / 3.0f), sin(light_angle + M_PI / 3.0f));
			float max_r = 0.0f;
			while (points.size() < 128)
			{
				auto current_iterator = open.begin();
				auto current = current_iterator->second;
				open.erase(current_iterator);
				if (visited.find(current) == visited.end())
				{
					visited.insert(current);
					points.push_back(hex_x * current.first + hex_y * current.second);
					float r = points.back().LenPwr();
					if (r > max_r)
						max_r = r;
					open.insert(std::make_pair((hex_x * current.first + hex_y * (current.second + 1)).LenPwr(), std::make_pair(current.first, current.second + 1)));
					open.insert(std::make_pair((hex_x * current.first + hex_y * (current.second - 1)).LenPwr(), std::make_pair(current.first, current.second - 1)));
					open.insert(std::make_pair((hex_x * (current.first - 1) + hex_y * (current.second + 1)).LenPwr(), std::make_pair(current.first - 1, current.second + 1)));
					open.insert(std::make_pair((hex_x * (current.first + 1) + hex_y * (current.second - 1)).LenPwr(), std::make_pair(current.first + 1, current.second - 1)));
					open.insert(std::make_pair((hex_x * (current.first - 1) + hex_y * current.second).LenPwr(), std::make_pair(current.first - 1, current.second)));
					open.insert(std::make_pair((hex_x * (current.first + 1) + hex_y * current.second).LenPwr(), std::make_pair(current.first + 1, current.second)));
				}
			}
			max_r = sqrt(max_r) + 0.5f;
			light_samples.resize(128 * 3);
			Vec3 light_x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
			Vec3 light_y = light.Cross(light_x).Normalize();
			for (int i = 0; i < 128; ++i)
			{
				Vec3 l = (light + (light_x * points[i].x + light_y * points[i].y) * light_size / max_r).Normalize();
				light_samples[i * 3] = l.x;
				light_samples[i * 3 + 1] = l.y;
				light_samples[i * 3 + 2] = l.z;
			}
		}

		// calculate rings shadow sample distribution
		if (input.isDown(Platform::KeyEvent::B) || light_samples.empty())
		{
			std::vector<Vec2> points;
			int ring = 1;
			float density = 3.0f;
			while (points.size() < 128)
			{
				int points_to_add = (2 * ring - 1) * density;
				if (points.size() + points_to_add > 128)
					points_to_add = 128 - points.size();
				for (int i = 0; i < points_to_add; ++i)
				{
					float angle = i * 2.0f * M_PI / points_to_add + ring;
					points.push_back(Vec2(cos(angle), sin(angle)) * ring);
				}
				++ring;
			}
			light_samples.resize(128 * 3);
			Vec3 light_x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
			Vec3 light_y = light.Cross(light_x).Normalize();
			for (int i = 0; i < 128; ++i)
			{
				Vec3 l = (light + (light_x * points[i].x + light_y * points[i].y) * light_size / ring).Normalize();
				light_samples[i * 3] = l.x;
				light_samples[i * 3 + 1] = l.y;
				light_samples[i * 3 + 2] = l.z;
			}
		}

		if (input.isDown(Platform::KeyEvent::V) || light_lookup.empty())
		{
			/*if (light_samples.size() == 128 * 3 && input.isDown(Platform::KeyEvent::V))
			{
				std::uniform_real_distribution<float> uni_dist_2pi(0.0f, M_PI * 2.0f);
				for (int i = 0; i < 128 * 3; i += 3)
				{
					Vec3 l(light_samples[i], light_samples[i + 1], light_samples[i + 2]);
					l *= Matrix3(uni_dist_2pi(random), light);
					light_samples[i] = l.x;
					light_samples[i + 1] = l.y;
					light_samples[i + 2] = l.z;
				}
			}*/

			unsigned short light_lookup_res_angle = 128;
			unsigned short light_lookup_res_distance = 64;
			// calculate shadow look-up table
			{
				LARGE_INTEGER freq, start, end;
				QueryPerformanceFrequency(&freq);
				QueryPerformanceCounter(&start);

				light_lookup.resize(light_lookup_res_angle * light_lookup_res_distance * 4);
				std::fill(light_lookup.begin(), light_lookup.end(), 0u);
				Vec3 light_x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
				Vec3 light_y = light.Cross(light_x).Normalize();
				for (int x = 0; x < light_lookup_res_angle; ++x)
				{
					for (int y = 0; y < light_lookup_res_distance; ++y)
					{
						float angle = x * M_PI * 2.0f / light_lookup_res_angle;
						Vec3 dir = light_x * cos(angle) + light_y * sin(angle);

						float dist = float(y) / light_lookup_res_distance * 2.0f - 1.0f;
						Vec3 off = light + dir * dist * light_size;
						off.Normalize();

						for (int j = 0; j < 4; ++j)
						{
							for (int i = 0; i < 32; ++i)
							{
								float dot = dir.Dot(off - Vec3(light_samples[(i + j * 32) * 3], light_samples[(i + j * 32) * 3 + 1], light_samples[(i + j * 32) * 3 + 2]));
								light_lookup[j + x * 4 + y * light_lookup_res_angle * 4] |= (dot > 0.0f ? 1u : 0u) << i;
							}
							if (y == 0)
								light_lookup[j + x * 4 + y * light_lookup_res_angle * 4] = 0u;
							if (y == light_lookup_res_distance - 1)
								light_lookup[j + x * 4 + y * light_lookup_res_angle * 4] = UINT32_MAX;
						}
					}
				}

				QueryPerformanceCounter(&end);
				double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
				Profiler::add("shadow-look-up", durationInSeconds);
			}

			// push shadow look-up table to gpu
			if (light_lookup_tex == 0)
			{
				light_lookup_tex.reset(new Texture());

				light_lookup_tex->w = light_lookup_res_angle;
				light_lookup_tex->h = light_lookup_res_distance;

				glGenTextures(1, &light_lookup_tex->texid);
				glBindTexture(GL_TEXTURE_2D, light_lookup_tex->texid);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, light_lookup_res_angle, light_lookup_res_distance, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, light_lookup.data());
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, light_lookup_tex->texid);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, light_lookup_res_angle, light_lookup_res_distance, GL_RGBA_INTEGER, GL_UNSIGNED_INT, light_lookup.data());
			}
		}
	}

	// stencils
	if (false)//stencil_prog->IsReady() && flat_stencil_prog->IsReady())
	{
		{
			// render flat stencil
			//glBindFramebuffer(GL_FRAMEBUFFER, flat_stencil->fb);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			flat_stencil_prog->Use();

			flat_stencil_prog->UniformMatrix4fv("proj", proj.data);
			flat_stencil_prog->UniformMatrix4fv("proj_inv", proj.Inverse().data);

			flat_stencil_prog->Uniform("light", light, 0.0f);

			flat_stencil_prog->Uniform("lsize", light_size);

			RenderSetup rs;
			rs.view = proj;

			ShaderMod mod(flat_stencil_prog, [proj, &rs](const std::shared_ptr<ShaderProgram>& prog) {
				prog->UniformMatrix4fv("transform", (rs.transform*proj).data);
				prog->UniformMatrix4fv("normal_transform", rs.transform.data);
			});

			rs.pushMod(mod);
			rs.pass = 1;

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			glCullFace(GL_FRONT);
			flat_stencil_prog->Uniform("value", -1.0f);
			for (int i = 0; i < 3; ++i)
			{
				flat_stencil_prog->Uniform("first", i);
				flat_stencil_prog->Uniform("second", (i + 1) % 3);
				flat_stencil_prog->Uniform("third", (i + 2) % 3);
				world->render(rs);
			}

			glCullFace(GL_BACK);
			flat_stencil_prog->Uniform("value", 1.0f);
			for (int i = 0; i < 3; ++i)
			{
				flat_stencil_prog->Uniform("first", i);
				flat_stencil_prog->Uniform("second", (i + 1) % 3);
				flat_stencil_prog->Uniform("third", (i + 2) % 3);
				world->render(rs);
			}

			rs.popMod();
		}

		{
			// render raytracing stencil
			//glBindFramebuffer(GL_FRAMEBUFFER, stencil->fb);

			uint32_t clear_value[4] = { UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX };
			glClearBufferuiv(GL_COLOR, 0, clear_value);

			stencil_prog->Use();

			stencil_prog->UniformMatrix4fv("proj", proj.data);
			stencil_prog->UniformMatrix4fv("proj_inv", proj.Inverse().data);

			stencil_prog->Uniform("zNear", near_z);
			stencil_prog->Uniform("zFar", far_z);

			//stencil_prog->Uniform("pixel", 1.0f / stencil->w, 1.0f / stencil->h);

			stencil_prog->Uniform("light", light, 0.0f);

			Vec3 light_x = light.Cross(Vec3(0.0f, 0.0f, 1.0f)).Normalize();
			Vec3 light_y = light.Cross(light_x).Normalize();

			stencil_prog->Uniform("light_x", light_x, 0.0f);
			stencil_prog->Uniform("light_y", light_y, 0.0f);

			stencil_prog->Uniform("lsize", light_size);

			stencil_prog->Uniform3fv("light_samples", light_samples);

			glActiveTexture(GL_TEXTURE1);
			stencil_prog->Uniform("depth", 1);
			//glBindTexture(GL_TEXTURE_2D, buffer->depth);
			glActiveTexture(GL_TEXTURE2);
			stencil_prog->Uniform("lookup", 2);
			if (light_lookup_tex != nullptr)
				glBindTexture(GL_TEXTURE_2D, light_lookup_tex->getGLTexID());
			glActiveTexture(GL_TEXTURE0);

			RenderSetup rs;
			rs.view = proj;

			ShaderMod mod(stencil_prog, [proj, &rs](const std::shared_ptr<ShaderProgram>& prog) {
				prog->UniformMatrix4fv("transform", (rs.transform*proj).data);
				prog->UniformMatrix4fv("normal_transform", rs.transform.data);
			});

			rs.pushMod(mod);
			rs.pass = 1;

			glDisable(GL_BLEND);
			glEnable(GL_COLOR_LOGIC_OP);
			glLogicOp(GL_AND);
			glEnable(GL_CULL_FACE);

			int min_i = 0;
			int max_i = 3;
			if (input.isDown(Platform::KeyEvent::T))
				max_i = 1;
			if (input.isDown(Platform::KeyEvent::Y))
			{
				min_i = 1;
				max_i = 2;
			}
			if (input.isDown(Platform::KeyEvent::U))
			{
				min_i = 2;
				max_i = 3;
			}
			for (int i = min_i; i < max_i; ++i)
			{
				stencil_prog->Uniform("first", i);
				stencil_prog->Uniform("second", (i + 1) % 3);
				stencil_prog->Uniform("third", (i + 2) % 3);
				world->render(rs);
			}

			glDisable(GL_COLOR_LOGIC_OP);
			glDepthMask(GL_TRUE);
			glEnable(GL_BLEND);

			rs.popMod();
		}
	}


	// render shaded geometry
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);

		deferred_fb->bind();
		glViewport(0, 0, buffer_w, buffer_h);

		RenderSetup rs;
		rs.view = proj;

		ShaderMod mod(shader_program, [this, proj, &rs](const std::shared_ptr<ShaderProgram>& prog) {
			prog->Uniform("light", light);
			prog->Uniform("diffuse", 0); // texture unit 0
			prog->UniformMatrix4fv("transform", (rs.transform*proj).data);
			prog->UniformMatrix4fv("normal_transform", rs.transform.data);
		});

		rs.pushMod(mod);
		rs.pass = 1;

		world->render(rs);

		rs.popMod();
	}


	// render to screen
	if (dof_prog->Use()) {
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		deferred_fb->unbind();
		glViewport(0, 0, view_w, view_h);

		dof_prog->Uniform("zNear", near_z);
		dof_prog->Uniform("zFar", far_z);

		dof_prog->Uniform("pixel", 1.0f / color_buf->w, 1.0f / color_buf->h);

		dof_prog->Uniform("max_kernel_size", std::ceilf(2.0f*color_buf->w/1000.0f));

		dof_prog->Uniform("x_size", 1.0f/supersample_x);
		dof_prog->Uniform("y_size", 1.0f/supersample_y);

		dof_prog->Uniform("diffuse", 0);
		glBindTexture(GL_TEXTURE_2D, color_buf->gl_texture_id);
		glActiveTexture(GL_TEXTURE1);
		dof_prog->Uniform("depth", 1);
		glBindTexture(GL_TEXTURE_2D, depth_buf->gl_texture_id);
		glActiveTexture(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
	}

	//glUseProgram(0);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	////set matrix to camera for editor
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMultMatrixf(proj.data);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//rs.pass = 2;

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer->depth, 0); //bind the scene depth

	//if (LineComponent::mode != 0)
	//	glEnable(GL_DEPTH_TEST);
	//LineComponent::render_all(rs);

	//if (LineComponent::mode == 2)
	//{
	//	rs.pass = 3;
	//	glDepthFunc(GL_NOTEQUAL);
	//	LineComponent::render_all(rs);
	//}

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);//unbind the scene depth to allow proper deferred rendering

	//glLoadIdentity();
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