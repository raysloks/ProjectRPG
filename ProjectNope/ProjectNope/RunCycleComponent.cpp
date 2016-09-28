#include "RunCycleComponent.h"

#include "Resource.h"
#include "StringResource.h"
#include "Texture.h"
#include "Mesh.h"

#include "NewEntity.h"

#include "Matrix3.h"

#include "FrameBuffer.h"
#include "ShaderProgram.h"

const AutoSerialFactory<RunCycleComponent> RunCycleComponent::_factory("RunCycleComponent");

RunCycleComponent::RunCycleComponent(const std::string& p) : Serializable(_factory.id), pointer(p) 
{
	Resource::load(pointer);
	g = 0;

	w = 212;
	h = 512;

	view_plane = 0.0f;

	play_speed = 0.05f;

	cutoff = 0.05f;
	diff = 0.05f;

	camera_mat = Matrix4::Perspective(70.6f, 60.0f/70.6f, 0.0f, 1.0f);
}

RunCycleComponent::RunCycleComponent(instream& is, bool full) : Serializable(_factory.id), Component(is, full)
{
	g = 0;
}

RunCycleComponent::~RunCycleComponent(void)
{
}

std::shared_ptr<Mesh> RunCycleComponent::constructMeshFromDepth(const std::vector<float>& d, bool inverse)
{
	std::shared_ptr<Mesh> mesh(new Mesh);
	mesh->sets.resize(1);
	for (auto j=d.begin();j!=d.end();++j)
	{
		int index = std::distance(d.begin(), j);
		int x = index%w;
		int y = index/w;
		float fx = float(x)*2.0f/w-1.0f;
		float fy = -float(y)*2.0f/h+1.0f;
		Vec3 world_pos = Vec3(tan(fx*0.5f*30.0f*M_PI/180.0f)**j, tan(fy*0.5f*70.6f*M_PI/180.0f)**j, *j);

		mesh->vert.push_back(Vertex(world_pos));
		mesh->uv.push_back(Vec2(fx/2.0f+0.5f, -fy/2.0f+0.5f));

		if (y>0 && x>0)
		{
			if (*(j-1)>=cutoff && *(j-w)>=cutoff)
			{
				if (*j>=cutoff)
				{
					if (abs(*(j-1)-*j)<=diff && abs(*(j-w)-*j)<=diff)
					{
						mesh->sets.front().push_back(Face(index-1, index, index-w));
						mesh->sets.front().back().ta = index-1;
						mesh->sets.front().back().tb = index;
						mesh->sets.front().back().tc = index-w;
						mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
					}
				}
				if (*(j-w-1)>=cutoff)
				{
					if (abs(*(j-1)-*(j-w-1))<=diff && abs(*(j-w)-*(j-w-1))<=diff)
					{
						mesh->sets.front().push_back(Face(index-1, index-w, index-w-1));
						mesh->sets.front().back().ta = index-1;
						mesh->sets.front().back().tb = index-w;
						mesh->sets.front().back().tc = index-w-1;
						mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
					}
				}
			}
			else
			{
				if (*j>=cutoff && *(j-w-1)>=cutoff)
				{
					if (*(j-1)>=cutoff)
					{
						if (abs(*j-*(j-1))<=diff && abs(*(j-w-1)-*(j-1))<=diff)
						{
							mesh->sets.front().push_back(Face(index, index-w-1, index-1));
							mesh->sets.front().back().ta = index;
							mesh->sets.front().back().tb = index-w-1;
							mesh->sets.front().back().tc = index-1;
							mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
						}
					}
					if (*(j-w)>=cutoff)
					{
						if (abs(*j-*(j-w))<=diff && abs(*(j-w-1)-*(j-w))<=diff)
						{
							mesh->sets.front().push_back(Face(index, index-w, index-w-1));
							mesh->sets.front().back().ta = index;
							mesh->sets.front().back().tb = index-w;
							mesh->sets.front().back().tc = index-w-1;
							mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
						}
					}
				}
			}
		}
	}
	if (!inverse)
	{
		for (auto i=mesh->sets.front().begin();i!=mesh->sets.front().end();++i)
		{
			i->flip();
		}
	}
	for (auto j=d.begin();j!=d.end();++j)
	{
		int index = std::distance(d.begin(), j);
		int x = index%w;
		int y = index/w;
		int samples = 0;
		if (x>0) {
			if (*(j-1)>=cutoff) {
				mesh->vert[index].n += (mesh->vert[index-1].p-mesh->vert[index].p).Normalize().Cross(Vec3(0.0f, 1.0f, 0.0f));
			} else {
				mesh->vert[index].n += Vec3(-1.0f, 0.0f, 0.0f);
			}
			++samples;
		}
		if (y>0) {
			if (*(j-w)>=cutoff) {
				mesh->vert[index].n += (mesh->vert[index-w].p-mesh->vert[index].p).Normalize().Cross(Vec3(1.0f, 0.0f, 0.0f));
			} else {
				mesh->vert[index].n += Vec3(0.0f, 1.0f, 0.0f);
			}
			++samples;
		}
		if (x<w) {
			if (*(j+1)>=cutoff) {
				mesh->vert[index].n += (mesh->vert[index+1].p-mesh->vert[index].p).Normalize().Cross(Vec3(0.0f, -1.0f, 0.0f));
			} else {
				mesh->vert[index].n += Vec3(1.0f, 0.0f, 0.0f);
			}
			++samples;
		}
		if (y<h) {
			if (*(j+w)>=cutoff) {
				mesh->vert[index].n += (mesh->vert[index+w].p-mesh->vert[index].p).Normalize().Cross(Vec3(-1.0f, 0.0f, 0.0f));
			} else {
				mesh->vert[index].n += Vec3(0.0f, -1.0f, 0.0f);
			}
			++samples;
		}
		mesh->vert[index].n /= samples;
		if (inverse)
			mesh->vert[index].n = -mesh->vert[index].n;
	}
	return mesh;
}

std::shared_ptr<Mesh> RunCycleComponent::constructMeshFromDepth(const std::vector<float>& near_depth, const std::vector<float>& far_depth)
{
	std::shared_ptr<Mesh> mesh(new Mesh);
	mesh->sets.resize(1);
	for (int di=0;di<2;++di)
	{
		const std::vector<float> * d = di==0 ? &near_depth : &far_depth;
		for (auto j=d->begin();j!=d->end();++j)
		{
			int index = std::distance(d->begin(), j)+di*w*h;
			int x = (index-di*w*h)%w;
			int y = (index-di*w*h)/w;
			float fx = float(x)*2.0f/w-1.0f;
			float fy = -float(y)*2.0f/h+1.0f;
			Vec3 world_pos = Vec3(tan(fx*0.5f*30.0f*M_PI/180.0f)**j, tan(fy*0.5f*70.6f*M_PI/180.0f)**j, *j);

			mesh->vert.push_back(Vertex(world_pos));
			mesh->uv.push_back(Vec2(fx/2.0f+0.5f, -fy/2.0f+0.5f));

			if (y>0 && x>0)
			{
				if (*(j-1)>=cutoff && *(j-w)>=cutoff)
				{
					if (*j>=cutoff)
					{
						if (abs(*(j-1)-*j)<=diff && abs(*(j-w)-*j)<=diff)
						{
							mesh->sets.front().push_back(Face(index-1, index, index-w));
							mesh->sets.front().back().ta = index-1;
							mesh->sets.front().back().tb = index;
							mesh->sets.front().back().tc = index-w;
							mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);

							if (di==0)
							{
								mesh->sets.front().back().flip();
								if (*(j-w-1)<cutoff || abs(*(j-1)-*(j-w-1))>diff || abs(*(j-w)-*(j-w-1))>diff)
								{
									mesh->sets.front().push_back(Face(index-w, index-1, index-1+w*h));
									mesh->sets.front().back().ta = index-w;
									mesh->sets.front().back().tb = index-1;
									mesh->sets.front().back().tc = index-1+w*h;
									mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
									mesh->sets.front().push_back(Face(index-1+w*h, index-w+w*h, index-w));
									mesh->sets.front().back().ta = index-1+w*h;
									mesh->sets.front().back().tb = index-w+w*h;
									mesh->sets.front().back().tc = index-w;
									mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
								}
							}
						}
					}
					if (*(j-w-1)>=cutoff)
					{
						if (abs(*(j-1)-*(j-w-1))<=diff && abs(*(j-w)-*(j-w-1))<=diff)
						{
							mesh->sets.front().push_back(Face(index-1, index-w, index-w-1));
							mesh->sets.front().back().ta = index-1;
							mesh->sets.front().back().tb = index-w;
							mesh->sets.front().back().tc = index-w-1;
							mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);

							if (di==0)
							{
								mesh->sets.front().back().flip();
								if (*j<cutoff || abs(*(j-1)-*j)>diff || abs(*(j-w)-*j)>diff)
								{
									mesh->sets.front().push_back(Face(index-w, index-1+w*h, index-1));
									mesh->sets.front().back().ta = index-w;
									mesh->sets.front().back().tb = index-1+w*h;
									mesh->sets.front().back().tc = index-1;
									mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
									mesh->sets.front().push_back(Face(index-1+w*h, index-w, index-w+w*h));
									mesh->sets.front().back().ta = index-1+w*h;
									mesh->sets.front().back().tb = index-w;
									mesh->sets.front().back().tc = index-w+w*h;
									mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
								}
							}
						}
					}
				}
				else
				{
					if (*j>=cutoff && *(j-w-1)>=cutoff)
					{
						if (*(j-1)>=cutoff)
						{
							if (abs(*j-*(j-1))<=diff && abs(*(j-w-1)-*(j-1))<=diff)
							{
								mesh->sets.front().push_back(Face(index, index-w-1, index-1));
								mesh->sets.front().back().ta = index;
								mesh->sets.front().back().tb = index-w-1;
								mesh->sets.front().back().tc = index-1;
								mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);

								if (di==0)
								{
									mesh->sets.front().back().flip();
									if (*(j-w)<cutoff || abs(*j-*(j-w))>diff || abs(*(j-w-1)-*(j-w))>diff)
									{
										mesh->sets.front().push_back(Face(index, index-w-1, index+w*h));
										mesh->sets.front().back().ta = index;
										mesh->sets.front().back().tb = index-w-1;
										mesh->sets.front().back().tc = index+w*h;
										mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
										mesh->sets.front().push_back(Face(index-w-1+w*h, index+w*h, index));
										mesh->sets.front().back().ta = index-w-1+w*h;
										mesh->sets.front().back().tb = index+w*h;
										mesh->sets.front().back().tc = index;
										mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
									}
								}
							}
						}
						if (*(j-w)>=cutoff)
						{
							if (abs(*j-*(j-w))<=diff && abs(*(j-w-1)-*(j-w))<=diff)
							{
								mesh->sets.front().push_back(Face(index, index-w, index-w-1));
								mesh->sets.front().back().ta = index;
								mesh->sets.front().back().tb = index-w;
								mesh->sets.front().back().tc = index-w-1;
								mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);

								if (di==0)
								{
									mesh->sets.front().back().flip();
									if (*(j-1)<cutoff || abs(*j-*(j-1))>diff || abs(*(j-w-1)-*(j-1))>diff)
									{
										mesh->sets.front().push_back(Face(index+w*h, index-w-1, index));
										mesh->sets.front().back().ta = index+w*h;
										mesh->sets.front().back().tb = index-w-1;
										mesh->sets.front().back().tc = index;
										mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
										mesh->sets.front().push_back(Face(index-w-1+w*h, index, index+w*h));
										mesh->sets.front().back().ta = index-w-1+w*h;
										mesh->sets.front().back().tb = index;
										mesh->sets.front().back().tc = index+w*h;
										mesh->sets.front().back().n = Vec3(0.0f, 0.0f, 1.0f);
									}
								}
							}
						}
					}
				}
			}
		}
		for (auto j=d->begin();j!=d->end();++j)
		{
			int index = std::distance(d->begin(), j)+di*w*h;
			int x = (index-di*w*h)%w;
			int y = (index-di*w*h)/w;
			int samples = 0;
			if (x>0) {
				if (*(j-1)>=cutoff) {
					mesh->vert[index].n += (mesh->vert[index-1].p-mesh->vert[index].p).Normalize().Cross(Vec3(0.0f, 1.0f, 0.0f));
				} else {
					mesh->vert[index].n += Vec3(-1.0f, 0.0f, 0.0f);
				}
				++samples;
			}
			if (y>0) {
				if (*(j-w)>=cutoff) {
					mesh->vert[index].n += (mesh->vert[index-w].p-mesh->vert[index].p).Normalize().Cross(Vec3(1.0f, 0.0f, 0.0f));
				} else {
					mesh->vert[index].n += Vec3(0.0f, 1.0f, 0.0f);
				}
				++samples;
			}
			if (x<w) {
				if (*(j+1)>=cutoff) {
					mesh->vert[index].n += (mesh->vert[index+1].p-mesh->vert[index].p).Normalize().Cross(Vec3(0.0f, -1.0f, 0.0f));
				} else {
					mesh->vert[index].n += Vec3(1.0f, 0.0f, 0.0f);
				}
				++samples;
			}
			if (y<h) {
				if (*(j+w)>=cutoff) {
					mesh->vert[index].n += (mesh->vert[index+w].p-mesh->vert[index].p).Normalize().Cross(Vec3(-1.0f, 0.0f, 0.0f));
				} else {
					mesh->vert[index].n += Vec3(0.0f, -1.0f, 0.0f);
				}
				++samples;
			}
			mesh->vert[index].n /= samples;
			if (di==1)
				mesh->vert[index].n = -mesh->vert[index].n;
		}
	}
	return mesh;
}

void RunCycleComponent::tick(float dTime)
{
	if (files.size()==0)
	{
		auto pstr = Resource::get<StringResource>(pointer);
		if (pstr!=0)
		{
			std::stringbuf buf(pstr->string);
			std::istream is(&buf);
			is >> file_format;
			std::string file;
			while (!is.eof())
			{
				is >> file;
				files.push_back(file);
			}
			depth.resize(files.size());
			pos.resize(files.size());
			blob.resize(files.size());
			textures.resize(files.size());
			meshes.resize(files.size());
		}
	}
	else
	{
		t += dTime * play_speed;

		if (t >= (files.size())/30.0f)
			t -= files.size()/30.0f;
	}

	std::vector<GLenum> buftype;
	buftype.push_back(GL_R32F);
	if (buffer==0)
		buffer.reset(new FrameBuffer(w, h, buftype, GL_NONE));

	if (analyze_program==0)
	{
		if (Resource::get<StringResource>("filters/analyze_vert.txt")!=0 && Resource::get<StringResource>("filters/analyze_frag.txt")!=0)
			analyze_program = std::shared_ptr<ShaderProgram>(new ShaderProgram(
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("filters/analyze_vert.txt")->string, GL_VERTEX_SHADER)), 
			std::shared_ptr<Shader>(new Shader(Resource::get<StringResource>("filters/analyze_frag.txt")->string, GL_FRAGMENT_SHADER))));
	}

	for (auto i=files.begin();i!=files.end();++i)
	{
		if ((depth.begin()+std::distance(files.begin(), i))->size()==0)
		{
			auto pstr = Resource::get<StringResource>(*i);
			if (pstr!=0)
			{
				std::stringbuf buf(pstr->string);
				if (file_format==0)
				{
					std::istream is(&buf);
					float f;
					while (!is.eof())
					{
						is >> f;
						if (f!=0.0f) {
							f /= 1000.0f;
							f += view_plane;
						}
						(depth.begin()+std::distance(files.begin(), i))->push_back(f);
					}
				}
				if (file_format==1)
				{
					std::vector<float> temp;
					instream is(&buf);
					unsigned short us;
					float f;
					while (!is.eof())
					{
						is >> us;
						f = us;
						if (f!=0.0f) {
							f /= 1000.0f;
							f += view_plane;
						}
						temp.push_back(f);
					}
					for (int x=0;x<h;++x)
					{
						for (int y=0;y<w;++y)
						{
							int index = y*h+x;

							(depth.begin()+std::distance(files.begin(), i))->push_back(*(temp.begin()+index));
						}
					}
				}
			}
		}
	}

	if (analyze_program!=0)
	{
		for (auto i=depth.begin();i!=depth.end();++i)
		{
			if ((pos.begin()+std::distance(depth.begin(), i))->size()==0)
			{
				if ((*i).size()>0)
				{
					//construct position field
					for (auto j=i->begin();j!=i->end();++j)
					{
						int index = std::distance(i->begin(), j);
						int x = index%w;
						int y = index/w;
						float fx = float(x)*2.0f/w-1.0f;
						float fy = -float(y)*2.0f/h+1.0f;
						Vec3 screen_pos = Vec3(fx/2.0f, fy, 1.0f);
						Vec3 view_plane_pos = screen_pos*camera_mat.Inverse();
						view_plane_pos.z = -view_plane_pos.z;
						view_plane_pos.Normalize();
						Vec3 world_pos = view_plane_pos * *j;

						(pos.begin()+std::distance(depth.begin(), i))->push_back(world_pos);
					}

					//construct depth texture
					{
						GLuint texid;
						glGenTextures(1, &texid);
						glBindTexture(GL_TEXTURE_2D, texid);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

						glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, i->data());

						std::shared_ptr<Texture> tex(new Texture);
						tex->w = w;
						tex->h = h;
						tex->texid = texid;

						std::string name = std::string("dat") + std::to_string((long long)std::distance(depth.begin(), i));

						*(textures.begin()+std::distance(depth.begin(), i)) = tex;
						Resource::add(name, tex);
					}

					//construct blobiness field
					{
						glBindFramebuffer(GL_FRAMEBUFFER, buffer->fb);

						glBindTexture(GL_TEXTURE_2D, (*(textures.begin()+std::distance(depth.begin(), i)))->getGLTexID());

						glUseProgram(analyze_program->prog);

						analyze_program->Uniform1i("depth", 0);

						glDisable(GL_DEPTH_TEST);

						glMatrixMode(GL_PROJECTION);
						glLoadIdentity();
						glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();

						glViewport(0, 0, w, h);

						glBegin(GL_QUADS);

						glTexCoord2f(0.0f, 0.0f);glVertex2f(0.0f, 0.0f);
						glTexCoord2f(1.0f, 0.0f);glVertex2f(1.0f, 0.0f);
						glTexCoord2f(1.0f, 1.0f);glVertex2f(1.0f, 1.0f);
						glTexCoord2f(0.0f, 1.0f);glVertex2f(0.0f, 1.0f);

						glEnd();

						glUseProgram(0);

						glBindFramebuffer(GL_FRAMEBUFFER, 0);

						glBindTexture(GL_TEXTURE_2D, buffer->tex[0]);
						(blob.begin()+std::distance(depth.begin(), i))->resize(w*h);
						glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, (blob.begin()+std::distance(depth.begin(), i))->data());
						//for (auto j=i->begin();j!=i->end();++j)
						//{
						//	int index = std::distance(i->begin(), j);
						//	int x = index%w;
						//	int y = index/w;

						//	std::vector<float> samples;

						//	float rad_step = 0.1f;
						//	for (float rad = 0.0f;rad<M_PI*2.0f;rad+=rad_step)
						//	{
						//		float max_dist = 200.0f;
						//		float dist_step = 1.0f;
						//		int px = x;
						//		int py = y;
						//		int pindex = index;
						//		float distance = 0.0f;
						//		for (float dist = 0.0f;dist<max_dist;dist+=dist_step)
						//		{
						//			int nx = x + cos(rad)*dist;
						//			int ny = y + sin(rad)*dist;
						//			int nindex = ny*w+nx;

						//			if (nx<0 || ny<0 || nx>=w || ny>=h)
						//				break;

						//			if (*(i->begin()+nindex)<=cutoff)
						//				break;
						//			if (abs(*(i->begin()+nindex)-*(i->begin()+pindex))>=diff)
						//				break;

						//			distance = (*((pos.begin()+std::distance(depth.begin(), i))->begin()+nindex)-*((pos.begin()+std::distance(depth.begin(), i))->begin()+index)).Len();

						//			px = nx;
						//			py = ny;
						//			pindex = nindex;
						//		}

						//		samples.push_back(distance);
						//	}

						//	std::sort(samples.begin(), samples.end());

						//	float blobiness;
						//	blobiness = 0.0f;
						//	for (int i=0;i<samples.size();++i)
						//	{
						//		blobiness += samples[i];
						//	}
						//	blobiness /= samples.size();
						//	/*if (samples.size()%2==0)
						//		blobiness = (samples[samples.size()/2]+samples[samples.size()/2-1])/2.0f;
						//	else
						//		blobiness = samples[samples.size()/2];*/
						//	(blob.begin()+std::distance(depth.begin(), i))->push_back(*j+blobiness);
						//}
					}

					//construct blob texture
					{
						GLuint texid;
						glGenTextures(1, &texid);
						glBindTexture(GL_TEXTURE_2D, texid);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

						glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (blob.begin()+std::distance(depth.begin(), i))->data());

						std::shared_ptr<Texture> tex(new Texture);
						tex->w = w;
						tex->h = h;
						tex->texid = texid;

						std::string name = std::string("datblob") + std::to_string((long long)std::distance(depth.begin(), i));

						Resource::add(name, tex);
					}

					//construct depth based mesh
					{
						std::shared_ptr<Mesh> mesh(constructMeshFromDepth(*i, *(blob.begin()+std::distance(depth.begin(), i))));
				
						std::string name = std::string("datmesh") + std::to_string((long long)std::distance(depth.begin(), i));

						*(meshes.begin()+std::distance(depth.begin(), i)) = mesh;
						Resource::add(name, mesh);
					}
				}
			}
		}
	}

	if (g==0)
		g = entity->getComponent<GraphicsComponent>();

	if (g!=0)
	{
		if (g->decs.items.size()==0)
		{
			Matrix4 local;
			local *= Matrix3((-90.0f+23.0f)*M_PI/180.0f, Vec3(1.0f, 0.0f, 0.0f));
			local *= Matrix4::Translation(Vec3(0.0f, 1.35f, 0.2f));

			g->decs.add(std::make_shared<Decorator>("data/assets/plane.gmdl", "data/assets/empty.tga", 0));
			g->decs.items[0]->final = local;

			Matrix3 scale;
			//scale *= 10.0f;
			g->decs.add(std::make_shared<Decorator>("data/assets/plane8x8.gmdl", "data/assets/props/misc/floor.tga", 0));
			g->decs.items[1]->final = scale;

			Matrix4 treadmill;
			//treadmill *= Matrix3(4.0f*M_PI/180.0f, Vec3(1.0f, 0.0f, 0.0f));
			treadmill *= Matrix4::Translation(Vec3(0.0f, 0.0f, 0.05f));
			g->decs.add(std::make_shared<Decorator>("data/assets/props/misc/treadmill.gmdl", "data/assets/test.tga", 0));
			g->decs.items[2]->final = treadmill;
		}
		if (g->decs.items.size()!=0)
		{
			g->decs.items[0]->_mesh = std::string("datmesh") + std::to_string((long long)(t*30.0f));
			//g->decs.items[0]->_tex[0] = std::string("datblob") + std::to_string((long long)(t*30.0f));
			/*Matrix3 rotation(dTime, Vec3(0.0f, 0.0f, 1.0f));
			g->decs.items[0]->final *= rotation;
			g->decs.items[1]->final *= rotation;*/
			auto mesh = Resource::get<Mesh>("data/assets/props/misc/treadmill.gmdl");
			if (mesh!=0)
			{
				for (auto i=mesh->uv.begin();i!=mesh->uv.end();++i)
				{
					(*i).y -= dTime*play_speed*(10.0f/3.0f)*0.95f*0.5f;
				}
				mesh->vbo_latest = false;
			}
		}
	}
}