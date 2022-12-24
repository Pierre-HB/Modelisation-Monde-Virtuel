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
    std::vector<GLuint> m_textures;
    int current_texture;
    bool use_texture;

    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<Color> colors;
    std::vector<vec2> uvs;
    std::vector<const char *> texture_files;
    std::vector<int> indexes;

    std::vector<Transform> oaks;
    std::vector<Transform> firs;
    std::vector<Transform> houses;
    Mesh oak;
    Mesh fir;
    Mesh house;

    bool left_arrow = false;
    bool right_arrow = false;

    bool t_press = false;
    bool h_press = false;
    bool show_tree = false;
    bool show_houses = false;

public:
    //! constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<int>& indexes) : App(width, height), current_texture(0), use_texture(false), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(std::vector<vec2>()), texture_files(), indexes(indexes), oaks(), firs() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<Color>& colors, const std::vector<int>& indexes) : App(width, height), current_texture(0), use_texture(false), positions(positions), normals(normals), colors(colors), uvs(std::vector<vec2>()), texture_files(), indexes(indexes), oaks(), firs(), houses() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, const char *texture_file, const std::vector<int>& indexes) : App(width, height), current_texture(0), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_files(std::vector<const char *>({texture_file})), indexes(indexes), oaks(), firs(), houses() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, const char *texture_file, const std::vector<int>& indexes, const std::vector<Transform>& oaks, const std::vector<Transform>& firs) : App(width, height), current_texture(0), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_files(std::vector<const char *>({texture_file})), indexes(indexes), oaks(oaks), firs(firs), houses() {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, const char *texture_file, const std::vector<int>& indexes, const std::vector<Transform>& oaks, const std::vector<Transform>& firs, const std::vector<Transform>& houses) : App(width, height), current_texture(0), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_files(std::vector<const char *>({texture_file})), indexes(indexes), oaks(oaks), firs(firs), houses(houses) {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<vec2>& uvs, std::vector<const char *> texture_files, const std::vector<int>& indexes, const std::vector<Transform>& oaks, const std::vector<Transform>& firs, const std::vector<Transform>& houses) : App(width, height), current_texture(0), use_texture(true), positions(positions), normals(normals), colors(std::vector<Color>()), uvs(uvs), texture_files(texture_files), indexes(indexes), oaks(oaks), firs(firs), houses(houses) {}
    
    //! creation des objets de l'application
    int init();
    
    //! destruction des objets de l'application
    int quit();
    
    //! dessiner une nouvelle image
    int render( );
};

#endif