#include "CreatePlane.h"
#include "Constants.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <thread>
#include <functional>

using namespace std;
using WL = Constants::WaterfallLake_WaterDrops;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Hash
// Description: Computes a pseudo-random hash value for a 2D point.
// Parameters:
//   - p: 2D vector input.
// Returns:
//   - A float representing the hash value.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Hash(glm::vec2 p)
{
    return glm::fract(sin(glm::dot(p, glm::vec2(127.1f, 311.7f))) * 43758.5453f);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Noise
// Description: Generates a smooth noise value for a given 2D point.
// Parameters:
//   - p: 2D vector input.
// Returns:
//   - A float representing the noise value.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Noise(glm::vec2 p)
{
    glm::vec2 i = glm::floor(p);
    glm::vec2 f = glm::fract(p);
    glm::vec2 u = f * f * (3.0f - 2.0f * f);

    return glm::mix(
        glm::mix(Hash(i + glm::vec2(0.0f, 0.0f)), Hash(i + glm::vec2(1.0f, 0.0f)), u.x),
        glm::mix(Hash(i + glm::vec2(0.0f, 1.0f)), Hash(i + glm::vec2(1.0f, 1.0f)), u.x),
        u.y
    );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: FBM
// Description: Computes Fractal Brownian Motion (FBM) for a given 2D point.
// Parameters:
//   - p: 2D vector input.
// Returns:
//   - A float representing the FBM value.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::FBM(glm::vec2 p)
{
    float total = 0.0f;
    float amplitude = 0.4f;
    float frequency = 1.0f;

    for (int i = 0; i < 4; ++i)
    {
        total += amplitude * Noise(p * frequency);
        frequency *= 2.0f;
        amplitude *= 0.5f;
    }

    return total;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Bezier
// Description: Computes a point on a cubic Bezier curve for a given t parameter.
// Parameters:
//   - t: The parameter (0 to 1) along the curve.
//   - p0, p1, p2, p3: Control points of the Bezier curve.
// Returns:
//   - A 3D vector representing the point on the curve.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Create::Bezier(
    float t,
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3)
{
    return
        p0 * glm::pow(1.0f - t, 3.0f) +
        p1 * 3.0f * t * glm::pow(1.0f - t, 2.0f) +
        p2 * 3.0f * glm::pow(t, 2.0f) * (1.0f - t) +
        p3 * glm::pow(t, 3.0f);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: BezierDerivative
// Description: Computes the derivative of a cubic Bezier curve for a given t parameter.
// Parameters:
//   - t: The parameter (0 to 1) along the curve.
//   - p0, p1, p2, p3: Control points of the Bezier curve.
// Returns:
//   - A 3D vector representing the derivative of the curve.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Create::BezierDerivative(
    float t,
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3)
{
    return
        3.0f * (1.0f - t) * (1.0f - t) * (p1 - p0) +
        6.0f * (1.0f - t) * t * (p2 - p1) +
        3.0f * t * t * (p3 - p2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Closest_Point_Bezier
// Description: Finds the closest point on a cubic Bezier curve to a given point.
// Parameters:
//   - vertexXZ: 2D position of the point to check against the curve.
//   - closest_t: Output parameter, stores the t value of the closest point on the curve.
//   - p0, p1, p2, p3: Control points of the Bezier curve.
// Returns:
//   - A float representing the distance to the closest point on the curve.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Closest_Point_Bezier(
    glm::vec2 vertexXZ,
    float& closest_t,
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3)
{
    const int initialSteps = 50;
    float minDistance = FLT_MAX;
    closest_t = 0.0f;

    for (int i = 0; i <= initialSteps; ++i)
    {
        float t = float(i) / float(initialSteps);
        glm::vec3 bezierPoint = Bezier(t, p0, p1, p2, p3);
        float distance = glm::length(vertexXZ - glm::vec2(bezierPoint.x, bezierPoint.z));

        if (distance < minDistance)
        {
            minDistance = distance;
            closest_t = t;
        }
    }

    const int maxIterations = 15;
    const float tolerance = 1e-4f;
    float stepSize = 0.0001f;

    for (int iter = 0; iter < maxIterations; ++iter)
    {
        glm::vec3 bezierPoint = Bezier(closest_t, p0, p1, p2, p3);
        glm::vec3 bezierTangent = BezierDerivative(closest_t, p0, p1, p2, p3);

        glm::vec2 gradient = glm::vec2(bezierTangent.x, bezierTangent.z);
        glm::vec2 direction = glm::vec2(bezierPoint.x, bezierPoint.z) - vertexXZ;

        float dotProduct = glm::dot(gradient, direction);
        closest_t -= stepSize * dotProduct;

        closest_t = glm::clamp(closest_t, 0.0f, 1.0f);

        float newDistance = glm::length(vertexXZ - glm::vec2(bezierPoint.x, bezierPoint.z));
        if (glm::abs(newDistance - minDistance) < tolerance)
        {
            break;
        }
        minDistance = newDistance;
    }

    return minDistance;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Displacement_Lake
// Description: Calculates the vertical displacement for a lake region.
// Parameters:
//   - position: 3D position of the vertex.
//   - center: Center of the lake region.
//   - radius: Radius of the lake region.
//   - h_max: Maximum height of the displacement.
// Returns:
//   - A float representing the vertical displacement.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Displacement_Lake(
    glm::vec3 position,
    glm::vec3 center, float radius, float h_max)
{
    float d = glm::distance(glm::vec2(position.x, position.z), glm::vec2(center.x, center.z)) / radius;
    float base_displacement = (d < 1.0f)
        ? (d * d * h_max) / 2.0f
        : (1.0f - (2.0f - d) * (2.0f - d) / 2.0f) * h_max;

    float noise_value = FBM(glm::vec2(position.x, position.z) * 0.35f) * 0.95f * h_max;
    float bridge_factor = glm::smoothstep(0.4f, 0.6f, d);
    base_displacement += noise_value * bridge_factor;

    return base_displacement;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Displacement_Waterfall
// Description: Calculates the vertical displacement for a waterfall region.
// Parameters:
//   - position: 3D position of the vertex.
//   - center: Center of the waterfall region.
//   - radius: Radius of influence for the waterfall.
//   - h_max: Maximum height of the displacement.
//   - p0, p1, p2, p3: Control points of the Bezier curve defining the waterfall's path.
// Returns:
//   - A float representing the vertical displacement.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Displacement_Waterfall(
    glm::vec3 position,
    const glm::vec3& center, float radius, float h_max,
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3)
{
    float closest_t;
    float d_bezier = Closest_Point_Bezier(glm::vec2(position.x, position.z), closest_t, p0, p1, p2, p3);

    glm::vec3 b_closest = Bezier(closest_t, p0, p1, p2, p3);
    float y_b_closest = b_closest.y;

    float d_ratio = glm::clamp(d_bezier / radius, 0.0f, 1.0f);
    float waterfall_effect = 1.0f - glm::sin(
        glm::pi<float>() / 2.0f - glm::clamp(d_ratio, 0.0f, 1.0f) * glm::pi<float>() / 2.0f
    );

    float offset = h_max * 1.15f;
    float displacement = glm::mix(y_b_closest + offset, position.y, waterfall_effect);

    float noise_factor = 1.0f - glm::smoothstep(0.0f, 0.15f, d_ratio);
    displacement += FBM(glm::vec2(position.x, position.z) * 0.35f) * noise_factor * h_max * 0.35f;

    float cut_radius = radius * 0.40f;
    if (d_bezier < cut_radius)
    {
        displacement -= 0.5 * h_max * (1.0f - d_bezier / cut_radius);
    }

    return displacement;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Displacement
// Description: Calculates the vertical displacement for both lake and waterfall regions.
// Parameters:
//   - position: 3D position of the vertex.
//   - center: Center of the displacement region.
//   - radius: Radius of influence for the region.
//   - h_max: Maximum height of the displacement.
//   - p0, p1, p2, p3: Control points of the Bezier curve defining the waterfall's path.
// Returns:
//   - A float representing the vertical displacement.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Create::Displacement(
    glm::vec3 position,
    const glm::vec3& center, float radius, float h_max,
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3)
{
    float closest_t;
    float d_bezier = Closest_Point_Bezier(glm::vec2(position.x, position.z), closest_t, p0, p1, p2, p3);
    float transition = glm::smoothstep(0.1f, 1.f, d_bezier / radius);

    float waterfall_displacement = Displacement_Waterfall(position, center, radius, h_max, p0, p1, p2, p3);
    float lake_displacement = Displacement_Lake(position, center, radius, h_max);

    float smooth_boundary = radius * 0.1f;
    if (d_bezier < smooth_boundary) 
    {
        return waterfall_displacement;
    }
    else if (d_bezier < smooth_boundary * 1.5f) 
    {
        return glm::mix(
            waterfall_displacement, 
            lake_displacement, 
            (d_bezier - smooth_boundary) / (smooth_boundary * 0.5f));
    }
    else 
    {
        return lake_displacement;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: CreateMesh
// Description: Creates a mesh from the specified vertices and indices.
// Parameters:
//   - name: Name of the mesh.
//   - vertices: List of vertices.
//   - indices: List of indices.
// Returns:
//   - A pointer to the created mesh.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mesh* Create::CreateMesh(
    const char* name,
    const vector<VertexFormat>& vertices,
    const vector<unsigned int>& indices)
{
    // Generate and bind VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind IBO
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, text_coord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, color));

    glBindVertexArray(0);

    CheckOpenGLError();

    // Create Mesh object
    Mesh* mesh = new Mesh(name);
    mesh->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    mesh->vertices = vertices;
    mesh->indices = indices;

    return mesh;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: CreateGridMesh
// Description: Creates a grid mesh with the specified dimensions and size.
// Parameters:
//   - name: Name of the mesh.
//   - gridX: Number of vertices along the X axis.
//   - gridZ: Number of vertices along the Z axis.
//   - gridSizeX: Size of the grid along the X axis.
//   - gridSizeZ: Size of the grid along the Z axis.
//   - outputPath: Optional path to save the grid to an OBJ file.
// Returns:
//   - A pointer to the created grid mesh.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mesh* Create::CreateGridMesh(
    const char* name,
    int gridX, int gridZ,
    float gridSizeX, float gridSizeZ,
    const char* textureName)
{
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    const float delta = 0.5f;

    float halfSizeX = gridSizeX * delta;
    float halfSizeZ = gridSizeZ * delta;

    float dx = gridSizeX / (gridX - 1);
    float dz = gridSizeZ / (gridZ - 1);

    float dTexX = 1.0f / (gridX - 1);
    float dTexZ = 1.0f / (gridZ - 1);

    const size_t numVertices = (size_t)(gridX * gridZ);
    const size_t numThreads = thread::hardware_concurrency();
    const size_t chunkSize = (numVertices + numThreads - 1) / numThreads;
    vector<vector<VertexFormat>> threadVertices(numThreads);

    auto computeVertices = [&](size_t threadIndex, size_t start, size_t end)
    {
        vector<VertexFormat>& localVertices = threadVertices[threadIndex];
        localVertices.reserve(end - start);

        for (size_t idx = start; idx < end; ++idx)
        {
			/// GRID INDEX AS A VECTOR ///
            int z = idx / gridX;
            int x = idx % gridX;

			/// POSITIONS ///
            glm::vec3 position(-halfSizeX + x * dx, 0.0f, halfSizeZ - z * dz);
            float displacement = Displacement(
                position,
                WL::CENTER, WL::RADIUS, WL::H_MAX,
                WL::CONTROL_P0, WL::CONTROL_P1, WL::CONTROL_P2, WL::CONTROL_P3);
            position.y += displacement;
            /// NORMALS ///
            float baseDisplacement = displacement;
            float dX = Displacement(position + glm::vec3(delta, 0, 0), WL::CENTER, WL::RADIUS, WL::H_MAX,
                WL::CONTROL_P0, WL::CONTROL_P1, WL::CONTROL_P2, WL::CONTROL_P3) - baseDisplacement;
            float dZ = Displacement(position + glm::vec3(0, 0, delta), WL::CENTER, WL::RADIUS, WL::H_MAX,
                WL::CONTROL_P0, WL::CONTROL_P1, WL::CONTROL_P2, WL::CONTROL_P3) - baseDisplacement;
            glm::vec3 normal = glm::normalize(glm::vec3(-dX, 2.0f * delta, -dZ));
			/// TEXTURE COORDINATES ///
            glm::vec2 texCoord(dTexX * x, dTexZ * z);
			/// COLORS ///
            glm::vec3 color(1.0f);

			/// ADD VERTEX ///
            localVertices.emplace_back(position, color, normal, texCoord);
        }
    };

    vector<thread> threads;

    for (size_t t = 0; t < numThreads; ++t)
    {
        size_t start = t * chunkSize;
        size_t end = min(start + chunkSize, numVertices);
        threads.emplace_back(computeVertices, t, start, end);
    }

    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    for (const auto& localVertices : threadVertices)
    {
        vertices.insert(
            vertices.end(),
            localVertices.begin(), 
            localVertices.end()
        );
    }

    for (int z = 0; z < gridZ - 1; ++z)
    {
        for (int x = 0; x < gridX - 1; ++x)
        {
            unsigned int topLeft = z * gridX + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * gridX + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* planeMesh = CreateMesh(name, vertices, indices);

	auto texture = TextureManager::GetTexture(textureName);
    if (texture)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        auto material = new Material();
        material->texture = texture;
        planeMesh->materials.push_back(material);

        for (auto& entry : planeMesh->meshEntries)
        {
            entry.materialIndex = 0;
        }

        planeMesh->useMaterial = true;
    }

    return planeMesh;
}
