#include "Scene.h"

Scene::Scene(size_t width, size_t height, size_t depth) : w(width), h(height), d(depth)
{
	chunks = new Chunk*[w * h * d];
	empty = new Chunk({ 0 });
	solid = new Chunk({ 1 });
	for (size_t i = 0; i < w * h * d; ++i)
	{
		chunks[i] = empty;
	}
}

Scene::~Scene()
{
	for (size_t i = 0; i < w * h * d; ++i)
	{
		if (chunks[i] != empty && chunks[i] != solid)
			delete chunks[i];
	}
	delete empty;
	delete solid;
	delete[] chunks;
}

Tile Scene::getTile(size_t x, size_t y, size_t z) const
{
	size_t cx = x >> CHUNK_SHIFT;
	size_t cy = y >> CHUNK_SHIFT;
	size_t cz = z >> CHUNK_SHIFT;
	if (cx >= w || cy >= h || cz >= d)
		return { 0 };
	return chunks[cx + cy * w + cz * w * h]->getTile(x - (cx << CHUNK_SHIFT), y - (cy << CHUNK_SHIFT), z - (cz << CHUNK_SHIFT));
}

void Scene::setTile(size_t x, size_t y, size_t z, Tile tile)
{
	size_t cx = x >> CHUNK_SHIFT;
	size_t cy = y >> CHUNK_SHIFT;
	size_t cz = z >> CHUNK_SHIFT;
	Chunk *& chunk = chunks[cx + cy * w + cz * w * h];
	if (chunk == empty)
		chunk = new Chunk(*empty);
	if (chunk == solid)
		chunk = new Chunk(*solid);
	chunk->setTile(x - (cx << CHUNK_SHIFT), y - (cy << CHUNK_SHIFT), z - (cz << CHUNK_SHIFT), tile);
}

std::shared_ptr<Mesh> Scene::createMesh() const
{
	std::shared_ptr<Mesh> mesh(new Mesh());

	mesh->uv.push_back(Vec2(0.0f, 0.0f));
	mesh->uv.push_back(Vec2(1.0f, 0.0f));
	mesh->uv.push_back(Vec2(1.0f, 1.0f));
	mesh->uv.push_back(Vec2(0.0f, 1.0f));

	mesh->sets.push_back(FaceSet());

	mesh->sets.back().nTextures = 1;

	size_t i = 0;
	for (size_t z = 1; z < d * CHUNK_SIZE - 1; ++z)
	{
		for (size_t y = 1; y < h * CHUNK_SIZE - 1; ++y)
		{
			for (size_t x = 1; x < w * CHUNK_SIZE - 1; ++x)
			{
				if (!getTile(x, y, z).solid)
				{
					if (getTile(x, y, z - 1).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x, y, z)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, 1.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, 1.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, 1.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, 1.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}

					if (getTile(x - 1, y, z).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x, y, z)));
						mesh->vert.back().n = Vec3(1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z)));
						mesh->vert.back().n = Vec3(1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y, z + 1)));
						mesh->vert.back().n = Vec3(1.0f, 0.0f, 0.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}

					if (getTile(x, y - 1, z).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z)));
						mesh->vert.back().n = Vec3(0.0f, 1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y, z)));
						mesh->vert.back().n = Vec3(0.0f, 1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 1.0f, 0.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}

					if (getTile(x, y, z + 1).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, -1.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, -1.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, -1.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, 0.0f, -1.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}

					if (getTile(x + 1, y, z).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(-1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z)));
						mesh->vert.back().n = Vec3(-1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z)));
						mesh->vert.back().n = Vec3(-1.0f, 0.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y, z + 1)));
						mesh->vert.back().n = Vec3(-1.0f, 0.0f, 0.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}

					if (getTile(x, y + 1, z).solid)
					{
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z)));
						mesh->vert.back().n = Vec3(0.0f, -1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z)));
						mesh->vert.back().n = Vec3(0.0f, -1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x + 1, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, -1.0f, 0.0f);
						mesh->vert.push_back(Vertex(Vec3(x, y + 1, z + 1)));
						mesh->vert.back().n = Vec3(0.0f, -1.0f, 0.0f);

						mesh->sets.back().uv_points.push_back(Face(0, 1, 2));
						mesh->sets.back().uv_points.push_back(Face(0, 2, 3));

						mesh->sets.back().vertices.push_back(Face(i, i + 1, i + 2));
						mesh->sets.back().vertices.push_back(Face(i, i + 2, i + 3));

						i += 4;
					}
				}
			}
		}
	}

	return mesh;
}