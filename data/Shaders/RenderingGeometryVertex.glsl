#version 410								
in vec4 Position; 								
in vec4 Color; 									
out vec4 outColor; 								

uniform mat4 projView; 							
uniform float time;								
uniform float heightScale;		
				
void main() 									
{ 												
	outColor = Color; 							
	vec4 P = Position;							
	P.y += sin( time + Position.x ) * cos( time + Position.z);
	gl_Position = projView * P; 				
}