
//! \file tuto7.cpp reprise de tuto6.cpp mais en derivant App::init(), App::quit() et bien sur App::render().


#include "simple_app.h"


    
// creation des objets de l'application
int SimpleApp::init( )
{
    // m_objet= read_mesh("data/cube.obj");
    oak = read_mesh("data/oak.obj");
    fir = read_mesh("data/fir.obj");
    house = read_mesh("data/home.obj");

    for(Transform model : oaks)
        oak.instance(model);
    for(Transform model : firs)
        fir.instance(model);
    for(Transform model : houses)
        house.instance(model);

    m_objet = Mesh(GL_TRIANGLES);
    if(use_texture)
        for(size_t i = 0; i < texture_files.size(); i++)
            m_textures.push_back(read_texture(0, texture_files[i]));
        // m_texture = read_texture(0, texture_file);


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
    m_camera.read_orbiter("camera");
    
    
    // etat openGL par defaut
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
    
    glClearDepth(1.f);                          // profondeur par defaut
    glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
    glEnable(GL_DEPTH_TEST);                    // activer le ztest

    std::cout << "============================" << std::endl;
    std::cout << "Controls : " << std::endl;
    std::cout << "s : save camera location" << std::endl;
    std::cout << "l : load camera location" << std::endl;
    std::cout << "t : toggle trees" << std::endl;
    std::cout << "h : toggles houses" << std::endl;
    std::cout << "-> : load the next texture" << std::endl;
    std::cout << "<- : load the previous texture" << std::endl;
    std::cout << "Texture are in the order :" << std::endl;
    std::cout << " - empty" << std::endl;
    std::cout << " - with paths" << std::endl;
    std::cout << " - with paths and streets" << std::endl;
    std::cout << " - with paths, streets and city circles" << std::endl;
    std::cout << " - with paths, streets, city circles and trees circles" << std::endl;


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
    
    if(key_state('s'))
    {
        // sauvegarde la camera
        m_camera.write_orbiter("camera");
        std::cout << "camera saved" << std::endl;
    }
    if(key_state('l'))
    {
        // charge la camera
        m_camera.read_orbiter("camera");
        std::cout << "camera loaded" << std::endl;
    }
    if(key_state('t'))
    {
        // toggle trees
        if(!t_press){
            show_tree = !show_tree;
            std::cout << "tree toggles : " << show_tree << std::endl;
        }
        t_press = true;
    }else{
        t_press = false;
    }
    if(key_state('h'))
    {
        // toggle trees
        if(!h_press){
            show_houses = !show_houses;
            std::cout << "houses toggles : " << show_houses << std::endl;
        }
        h_press = true;
    }else{
        h_press = false;
    }
    if(key_state(80 | 1 << 30)){
        //80 | 1 << 30 magic number for left arrow
        if(!left_arrow){
            current_texture = (current_texture + m_textures.size() - 1) % m_textures.size();
            std::cout << "Previous texture loaded : " << current_texture << std::endl;
        }
        left_arrow = true;
    }else{
        left_arrow = false;
    }
    if(key_state(79 | 1 << 30)){
        //79 | 1 << 30 magic number for right arrow
        if(!right_arrow){
            current_texture = (current_texture + 1) % m_textures.size();
            std::cout << "Next texture loaded : " << current_texture << std::endl;
        }
        right_arrow = true;
    }else{
        right_arrow = false;
    }

    // SDLK_RIGHT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT),
    // SDLK_LEFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT),
    // SDLK_DOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN),
    // SDLK_UP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP),
    
    // int write_orbiter( const char *filename )


    // affiche l'objet pour le point de vue de la camera et avec une texture
    const bool debug_normal = false;
    if(debug_normal){
        DrawParam param;
        Transform view= m_camera.view();
        Transform projection= m_camera.projection(window_width(), window_height(), 45);
        
        float length_normal = 0.1;
        param.debug_normals(length_normal);
        param.texture(m_textures[current_texture]);
        param.model(Identity()).view(view).projection(projection);
        param.draw(m_objet);
    }else{
        if(use_texture)
            draw(m_objet, m_camera, m_textures[current_texture]);
        else
            draw(m_objet, m_camera);
    }

    if(show_tree){
        for(TriangleGroup tg : oak.groups())
            draw(tg, oak, Identity(), m_camera);
    
        for(TriangleGroup tg : fir.groups())
            draw(tg, fir, Identity(), m_camera);
    }

    if(show_houses){
        for(TriangleGroup tg : house.groups())
            draw(tg, house, Identity(), m_camera);
    }
    
    
    
    

    
    
    return 1;
}


