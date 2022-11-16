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
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    std::vector<Color> colors;
    std::vector<int> indexes;
public:
    //! constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<int>& indexes) : App(width, height), positions(positions), normals(normals), colors(std::vector<Color>()), indexes(indexes) {}
    SimpleApp(const int width, const int height, const std::vector<vec3>& positions, const std::vector<vec3>& normals, const std::vector<Color>& colors, const std::vector<int>& indexes) : App(width, height), positions(positions), normals(normals), colors(colors), indexes(indexes) {}
    
    //! creation des objets de l'application
    int init();
    
    //! destruction des objets de l'application
    int quit();
    
    //! dessiner une nouvelle image
    int render( );
};

#endif