
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;

#ifdef USE_COLOR
    layout(location= 3) in vec4 color;
    out vec4 vertex_color;
#endif

#ifdef USE_INSTANCES
    layout(location= 5) in mat4 instance;
#endif

uniform mat4 mvpMatrix;

void main( )
{
#ifdef USE_INSTANCES
    gl_Position= mvpMatrix * transpose(instance) * vec4(position, 1);
#else
    gl_Position= mvpMatrix * vec4(position, 1);
#endif
    
#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif

#ifdef FRAGMENT_SHADER

#ifdef USE_COLOR
    in vec4 vertex_color;
#endif

uniform vec4 mesh_color;

out vec4 fragment_color;

void main( )
{
    vec4 color= mesh_color;
#ifdef USE_COLOR
    color= vertex_color;
#endif
    
    fragment_color= color;
}
#endif
