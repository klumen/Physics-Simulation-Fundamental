#include "Model.h"

Model::Model(const std::string& path)
{
	load(path);
}

void Model::draw(Shader& shader) const
{
	for (auto& mesh : meshes)
		mesh.draw(shader);
}

Mesh* Model::find_mesh(const std::string& name)
{
	for (auto& mesh : meshes)
		if (mesh.name == name)
			return &mesh;

	return nullptr;
}

void Model::load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenBoundingBoxes);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(process_mesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
		process_node(node->mChildren[i], scene);
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	AABB box;

	name = mesh->mName.C_Str();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector3;

		vector3.x = mesh->mVertices[i].x;
		vector3.y = mesh->mVertices[i].y;
		vector3.z = mesh->mVertices[i].z;
		vertex.position = vector3;

		vector3 = glm::vec3(0.f, 0.f, 0.f);
		if (mesh->HasNormals())
		{
			vector3.x = mesh->mNormals[i].x;
			vector3.y = mesh->mNormals[i].y;
			vector3.z = mesh->mNormals[i].z;
		}
		vertex.normal = vector3;

		glm::vec2 vector2(0.f, 0.f);
		if (mesh->mTextureCoords[0])
		{
			vector2.x = mesh->mTextureCoords[0][i].x;
			vector2.y = mesh->mTextureCoords[0][i].y;
		}
		vertex.texCoord = vector2;

		vertices.emplace_back(std::move(vertex));
	}
	
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuse = load_textures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuse.begin(), diffuse.end());
		std::vector<Texture> specular = load_textures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular.begin(), specular.end());
		std::vector<Texture> normal = load_textures(material, aiTextureType_NORMALS, "normal");
		textures.insert(textures.end(), normal.begin(), normal.end());
		std::vector<Texture> height = load_textures(material, aiTextureType_HEIGHT, "height");
		textures.insert(textures.end(), height.begin(), height.end());
		std::vector<Texture> ambientOcclusion = load_textures(material, aiTextureType_AMBIENT_OCCLUSION, "ambientOcclusion");
		textures.insert(textures.end(), ambientOcclusion.begin(), ambientOcclusion.end());
	}

	box.max.x = mesh->mAABB.mMax.x;
	box.max.y = mesh->mAABB.mMax.y;
	box.max.z = mesh->mAABB.mMax.z;
	box.min.x = mesh->mAABB.mMin.x;
	box.min.y = mesh->mAABB.mMin.y;
	box.min.z = mesh->mAABB.mMin.z;

	return Mesh(name, vertices, indices, textures, box);
}

std::vector<Texture> Model::load_textures(aiMaterial* mtl, aiTextureType type, const std::string& typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mtl->GetTextureCount(type); i++)
	{
		aiString str;
		mtl->GetTexture(type, i, &str);

		bool skip = false;
		for (auto& texture : textures_loaded)
		{
			if (texture.name == str.C_Str())
			{
				textures.emplace_back(texture);
				skip = true;
				break;
			}
		}
		if (skip)
			continue;

		Texture texture;
		texture.id = load_texture(directory, str.C_Str());
		texture.type = typeName;
		texture.name = str.C_Str();
		textures.emplace_back(texture);
		textures_loaded.emplace_back(std::move(texture));
	}

	return textures;
}

unsigned int load_texture(const std::string& directory, const std::string& name)
{
	std::string path = directory + "/" + name;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		else if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture0" << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}