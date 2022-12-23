#ifndef _SIMPLE_APP_H
#define _SIMPLE_APP_H

#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"        
#include "app.h"        // classe Application a deriver

class SimpleApp : public App
{
protected:
    Mesh m_objet;
    Orbiter m_camera;
    GLuint m_texture;
    bool use_texture;
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<Color> colors;
    std::vector<vec2> uvs;
    const char *texture_file;
    std::vector<int> indexes;

    std::vector<Transform> oaks;
    Mesh oak;
public:
    //! constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<int>& indexes) : App(width, height), use_texture(false), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(std::vector<vec2>()), texture_file(), indexes(indexes), oaks() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<Color>& colors, const std::vector<int>& indexes) : App(width, height), use_texture(false), positions(positions), normals(normals), colors(colors), uvs(std::vector<vec2>()), texture_file(), indexes(indexes), oaks() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, const char *texture_file, const std::vector<int>& indexes) : App(width, height), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_file(texture_file), indexes(indexes), oaks() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, const char *texture_file, const std::vector<int>& indexes, const std::vector<Transform>& oaks) : App(width, height), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_file(texture_file), indexes(indexes), oaks(oaks) {}
    
    //! creation des objets de l'application
    int init();
    
    //! destruction des objets de l'application
    int quit();
    
    //! dessiner une nouvelle image
    int render( );
};

#endif