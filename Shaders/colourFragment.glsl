# version 150 core

in Vertex 
{
    vec4 colour ;
} IN ;

out vec4 fragColour ;

void main ( void ) 
{
    fragColour = IN . colour ;
}