#include "CubeMap.h"
#include "Constants.h"
#include "utils/memory_utils.h"

#include <iostream>
#include <unordered_map>


using namespace std;
using CM = Constants::CubeMap;


CubeMap::CubeMap(WindowObject* window) : window(window)
{
    framebuffer_object = 0;
	resolution = glm::ivec2(0);

    color_texture = depth_texture = 0;
    cube_texture = cube_angle = 0.0f,

    width = height = 0;
    shadow_type = 0;
}

CubeMap::~CubeMap()
{
    if (framebuffer_object) glDeleteFramebuffers(1, &framebuffer_object);
    if (cube_texture) glDeleteTextures(1, &cube_texture);
    if (color_texture) glDeleteTextures(1, &color_texture);
    if (depth_texture) glDeleteTextures(1, &depth_texture);
    if (window)
    {
        delete window;
		window = nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: ResizeBuffers
// Description: Resizes the G-buffer textures.
// Parameters:
//   - width: New width of the G-buffer textures.
//   - height: New height of the G-buffer textures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CubeMap::ResizeBuffers(int width, int height)
{
    this->width = width;
    this->height = height;
    resolution = glm::ivec2(width, height);

    if (color_texture)
    {
        glDeleteTextures(1, &color_texture);
        color_texture = 0;
    }
    if (depth_texture)
    {
        glDeleteTextures(1, &depth_texture);
        depth_texture = 0;
    }

    CreateFramebuffer(width, height);
    cout << "CubeMap buffers resized to " << width << "x" << height << endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: Init
// Description: Initializes the CubeMap by loading its textures and setting up a framebuffer for rendering.
// Parameters:
//   - windowObj: Pointer to the WindowObject instance that provides context.
//   - w: Width of the CubeMap textures and framebuffer.
//   - h: Height of the CubeMap textures and framebuffer.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CubeMap::Init(
    WindowObject* window,
    int w, int h)
{
    // Store the window object and dimensions
    this->window = window;
    this->width = w;
    this->height = h;

    // Path to the CubeMap texture directory
    string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cubemap");

    // Load textures for the CubeMap faces
    cube_texture = UploadCubeMapTexture(
        PATH_JOIN(texturePath, "pos_x.png"), // +X face
        PATH_JOIN(texturePath, "pos_y.png"), // +Y face
        PATH_JOIN(texturePath, "pos_z.png"), // +Z face
        PATH_JOIN(texturePath, "neg_x.png"), // -X face
        PATH_JOIN(texturePath, "neg_y.png"), // -Y face
        PATH_JOIN(texturePath, "neg_z.png")  // -Z face
    );

    // Check if texture upload was successful
    if (cube_texture == 0)
    {
        cerr << "Error: Failed to initialize CubeMap textures." << endl;
    }

    // Create the framebuffer for rendering into the CubeMap
    CreateFramebuffer(width, height);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: UploadCubeMapTexture
// Description: Loads six images into the CubeMap texture (one for each face) and configures its settings.
// Parameters:
//   - posx: File path for the +X face texture.
//   - posy: File path for the +Y face texture.
//   - posz: File path for the +Z face texture.
//   - negx: File path for the -X face texture.
//   - negy: File path for the -Y face texture.
//   - negz: File path for the -Z face texture.
// Returns:
//   - The ID of the created CubeMap texture. Returns 0 if loading fails.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int CubeMap::UploadCubeMapTexture(
    const string& posx, const string& posy, const string& posz,
    const string& negx, const string& negy, const string& negz)
{
    int width, height, chn;

    unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
    unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
    unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
    unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
    unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
    unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

    glGenTextures(1, &cube_texture);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    float maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // free memory
    SAFE_FREE(data_posx);
    SAFE_FREE(data_posy);
    SAFE_FREE(data_posz);
    SAFE_FREE(data_negx);
    SAFE_FREE(data_negy);
    SAFE_FREE(data_negz);

    return cube_texture;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function: CreateFramebuffer
// Description: Creates a framebuffer and attaches a color texture and depth texture for rendering into the CubeMap.
// Parameters:
//   - width: Width of the framebuffer textures.
//   - height: Height of the framebuffer textures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CubeMap::CreateFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &framebuffer_object);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (color_texture)
    {
        //cubemap params
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (GLEW_EXT_texture_filter_anisotropic) {
            float maxAnisotropy;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Bind the color textures to the framebuffer as a color attachments
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        std::vector<GLenum> draw_textures;
        draw_textures.push_back(GL_COLOR_ATTACHMENT0);
        glDrawBuffers(draw_textures.size(), &draw_textures[0]);

    }

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_texture);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
    {
        std::cerr << "Error: Framebuffer is not complete! Status: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
