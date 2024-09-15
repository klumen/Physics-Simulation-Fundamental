#include "SoftBody.h"

SoftBody::SoftBody() : gravity(0.f, -9.8f, 0.f)
{
    // Read the house model from files.
    // The model is from Jonathan Schewchuk's Stellar lib.
    {
        std::ifstream file("D:/Unity project/games103/Assets/house2.ele");
        std::stringstream fileStream;
        fileStream << file.rdbuf();
        std::vector<std::string> fileContent;
        std::string token;
        while (fileStream >> token)
            fileContent.emplace_back(token);

        tetNr = std::stoul(fileContent[0]);
        tets.resize(tetNr);
        for (unsigned int i = 0; i < tetNr; i++)
        {
            tets[i][0] = std::stoul(fileContent[i * 5 + 4]) - 1;
            tets[i][1] = std::stoul(fileContent[i * 5 + 5]) - 1;
            tets[i][2] = std::stoul(fileContent[i * 5 + 6]) - 1;
            tets[i][3] = std::stoul(fileContent[i * 5 + 7]) - 1;
        }
    }

    {
        std::ifstream file("D:/Unity project/games103/Assets/house2.node");
        std::stringstream fileStream;
        fileStream << file.rdbuf();
        std::vector<std::string> fileContent;
        std::string token;
        while (fileStream >> token)
            fileContent.emplace_back(token);

        particleNr = std::stoul(fileContent[0]);
        x.resize(particleNr);
        for (unsigned int i = 0; i < particleNr; i++)
        {
            x[i].x = std::stof(fileContent[i * 5 + 5]) * 0.4f;
            x[i].y = std::stof(fileContent[i * 5 + 6]) * 0.4f;
            x[i].z = std::stof(fileContent[i * 5 + 7]) * 0.4f;
        }

        glm::vec3 center(0.f);
        for (auto& x_i : x)
            center += x_i;
        center /= particleNr;

        m.resize(particleNr);
        for (unsigned int i = 0; i < particleNr; i++)
        {
            x[i] -= center;
            std::swap(x[i].y, x[i].z);
            m[i] = 1.f;
        }
    }

    /*tet_number=1;
    Tet = new int[tet_number*4];
    Tet[0]=0;
    Tet[1]=1;
    Tet[2]=2;
    Tet[3]=3;

    number=4;
    X = new Vector3[number];
    V = new Vector3[number];
    Force = new Vector3[number];
    X[0]= new Vector3(0, 0, 0);
    X[1]= new Vector3(1, 0, 0);
    X[2]= new Vector3(0, 1, 0);
    X[3]= new Vector3(0, 0, 1);*/

    std::vector<glm::vec3> vertices(tetNr * 12);
    unsigned int vertexNr = 0;
    for (unsigned int i = 0; i < tetNr; i++)
    {
        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][2]];
        vertices[vertexNr++] = x[tets[i][1]];

        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][3]];
        vertices[vertexNr++] = x[tets[i][2]];

        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][1]];
        vertices[vertexNr++] = x[tets[i][3]];

        vertices[vertexNr++] = x[tets[i][1]];
        vertices[vertexNr++] = x[tets[i][2]];
        vertices[vertexNr++] = x[tets[i][3]];
    }

    std::vector<unsigned int> indices(tetNr * 12);
    for (unsigned int i = 0; i < tetNr * 4; i++)
    {
        indices[i * 3 + 0] = i * 3 + 0;
        indices[i * 3 + 1] = i * 3 + 1;
        indices[i * 3 + 2] = i * 3 + 2;
    }

    mesh = new Mesh("house", vertices, indices, std::vector<Texture>(), {glm::vec3(0.f), glm::vec3(0.f)});
    mesh->recalculate_normals();

    v.resize(particleNr);
    f.resize(particleNr);

    Dm_inv.resize(tetNr);
    for (unsigned int i = 0; i < tetNr; i++)
    {
        const glm::vec3& x0 = x[tets[i][0]];
        const glm::vec3& x1 = x[tets[i][1]];
        const glm::vec3& x2 = x[tets[i][2]];
        const glm::vec3& x3 = x[tets[i][3]];

        glm::vec3 x10 = x1 - x0;
        glm::vec3 x20 = x2 - x0;
        glm::vec3 x30 = x3 - x0;
        glm::mat3 Dm(x10, x20, x30);

        Dm_inv[i] = glm::inverse(Dm);
    }
}

void SoftBody::update(float deltaTime, Shader& shader)
{
    if (mesh == nullptr)
        return;

    FVM(deltaTime, shader);
}

void SoftBody::FVM(float deltaTime, Shader& shader)
{
    deltaTime = 0.003f;

    for (unsigned int i = 0; i < particleNr; i++)
        f[i] = m[i] * gravity;

    for (unsigned int i = 0; i < tetNr; i++)
    {
        const glm::vec3& x0 = x[tets[i][0]];
        const glm::vec3& x1 = x[tets[i][1]];
        const glm::vec3& x2 = x[tets[i][2]];
        const glm::vec3& x3 = x[tets[i][3]];

        glm::vec3 x10 = x1 - x0;
        glm::vec3 x20 = x2 - x0;
        glm::vec3 x30 = x3 - x0;

        glm::mat3 F = glm::mat3(x10, x20, x30) * Dm_inv[i];

        glm::mat3 G = 0.5f * (glm::transpose(F) * F - glm::mat3(1.f));

        glm::mat3 S = 2.f * mu * G + lambda * (G[0][0] + G[1][1] + G[2][2]) * glm::mat3(1.f);

        glm::mat3 P = F * glm::transpose(S);

        glm::mat3 fs = -1.f / (6.f * glm::determinant(Dm_inv[i])) * P * glm::transpose(Dm_inv[i]);
        
        f[tets[i][0]] += -fs[0] - fs[1] - fs[2];
        f[tets[i][1]] += fs[0];
        f[tets[i][2]] += fs[1];
        f[tets[i][3]] += fs[2];
    }

    smooth_v();

    for (unsigned int i = 0; i < particleNr; i++)
    {
        v[i] += deltaTime * (f[i] / m[i]);
        v[i] *= exp(-deltaTime * damping);

        collision(glm::vec3(0.f, -3.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        
        x[i] += deltaTime * v[i];
    }

    std::vector<glm::vec3> vertices(tetNr * 12);
    unsigned int vertexNr = 0;
    for (unsigned int i = 0; i < tetNr; i++)
    {
        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][2]];
        vertices[vertexNr++] = x[tets[i][1]];

        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][3]];
        vertices[vertexNr++] = x[tets[i][2]];

        vertices[vertexNr++] = x[tets[i][0]];
        vertices[vertexNr++] = x[tets[i][1]];
        vertices[vertexNr++] = x[tets[i][3]];

        vertices[vertexNr++] = x[tets[i][1]];
        vertices[vertexNr++] = x[tets[i][2]];
        vertices[vertexNr++] = x[tets[i][3]];
    }
    for (unsigned int i = 0; i < vertices.size(); i++)
        mesh->vertices[i].position = vertices[i];
    mesh->recalculate_normals();
}

void SoftBody::collision(const glm::vec3& P, const glm::vec3& N)
{
    for (unsigned int i = 0; i < particleNr; i++)
        if (glm::dot(x[i] - P, N) < 0 && glm::dot(v[i], N) < 0)
            v[i] *= -0.5f;
}

void SoftBody::smooth_v()
{
    std::vector<glm::vec3> v_sum(particleNr, glm::vec3(0.f));
    std::vector<float> v_num(particleNr, 0.f);

    for (unsigned int i = 0; i < tetNr; i++)
    {
        glm::vec3 sum = v[tets[i][0]] + v[tets[i][1]] + v[tets[i][2]] + v[tets[i][3]];

        v_sum[tets[i][0]] += sum;
        v_sum[tets[i][1]] += sum;
        v_sum[tets[i][2]] += sum;
        v_sum[tets[i][3]] += sum;
        v_num[tets[i][0]] += 4;
        v_num[tets[i][1]] += 4;
        v_num[tets[i][2]] += 4;
        v_num[tets[i][3]] += 4;
    }

    for (unsigned int i = 0; i < particleNr; i++)
        v[i] = 0.9f * v[i] + 0.1f * v_sum[i] / v_num[i];
}