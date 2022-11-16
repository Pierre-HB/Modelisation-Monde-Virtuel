
//! \file tuto7.cpp reprise de tuto6.cpp mais en derivant App::init(), App::quit() et bien sur App::render().


#include "simple_app.h"


    
// creation des objets de l'application
int SimpleApp::init( )
{
    // m_objet= read_mesh("data/cube.obj");
    m_objet = Mesh(GL_TRIANGLES);
    if(use_texture)
        m_texture = read_texture(0, texture_file);


    //Carefull, the normal of each vertex MUST be in the same direction ad the triangle normal
    assert(positions.size() == normals.size());
    for(size_t i = 0; i < positions.size(); i++){
        m_objet.normal(normals[i]);
        if(colors.size() != 0)
            m_objet.color(colors[i]);
        if(uvs.size() != 0)
            m_objet.texcoord(uvs[i]);
        m_objet.vertex(positions[i]);
        
    }
    for(size_t i = 0; i < indexes.size(); i+=3){
        m_objet.triangle(indexes[i], indexes[i+1], indexes[i+2]);
    }
    
    Point pmin, pmax;
    m_objet.bounds(pmin, pmax);
    m_camera.lookat(pmin, pmax);
    
    
    // etat openGL par defaut
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
    
    glClearDepth(1.f);                          // profondeur par defaut
    glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
    glEnable(GL_DEPTH_TEST);                    // activer le ztest

    return 0;   // ras, pas d'erreur
}
    
// destruction des objets de l'application
int SimpleApp::quit( )
{
    m_objet.release();
    
    return 0;
}
    
// dessiner une nouvelle image
int SimpleApp::render( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // deplace la camera
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
        m_camera.rotation(mx, my);
    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
        m_camera.move(mx);
    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
    
    // affiche l'objet pour le point de vue de la camera et avec une texture
    const bool debug_normal = true;
    if(debug_normal){
        DrawParam param;
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        // float length_normal = 0.01;
        // param.debug_normals(length_normal);
        param.texture(m_texture);
        param.model(Identity()).view(view).projection(projection);
        param.draw(m_objet);
    }else{
        if(use_texture)
            draw(m_objet, m_camera, m_texture);
        else
            draw(m_objet, m_camera);
    }
    

    
    
    return 1;
}


