///////////////////////////////////////////////////////////////////////////////////////////////////
///
///     GLTestApp OpenGL 2.0 Simple Vertex Shader
///
///     Copyright 2012, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

uniform    mat4 worldMatrix;
uniform    mat4 viewMatrix;
uniform    mat4 projectionMatrix;
attribute  vec3 in_Position;
attribute  vec4 in_Color;
attribute  vec2 in_TexCoord;
varying    vec4 v_Color;
varying    vec2 v_TexCoord;

void main(void)
{
    vec4 position = vec4(in_Position, 1.0);
    position = position * projectionMatrix * viewMatrix * worldMatrix;
    gl_Position = position;

    v_TexCoord = in_TexCoord;
    v_Color = in_Color;
}

