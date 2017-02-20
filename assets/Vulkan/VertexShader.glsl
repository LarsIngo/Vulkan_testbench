// buffer inputs
#ifdef NORMAL
	layout(binding=NORMAL) buffer nor { vec4 normal_in[]; };
	layout(location=NORMAL) out vec4 normal_out;
#endif

#ifdef TEXTCOORD
	layout(binding=TEXTCOORD) buffer text { vec2 uv_in[]; };
	layout(location=TEXTCOORD) out vec2 uv_out;
#endif

layout(binding=POSITION) buffer pos { vec4 position_in[]; };

layout(binding=TRANSLATION) buffer TRANSLATION_NAME
{
	vec4 translate;
};

layout(binding=DIFFUSE_TINT) buffer DIFFUSE_TINT_NAME
{
	vec4 diffuseTint;
};

layout(location=8) out vec4 debug_out;

void main() {
    uint vID = gl_VertexIndex;
    vec4 pos = position_in[vID];
    
    //debug_out = translate;
    debug_out = vec4(vID == 0,0,0,1); 
    
    #ifdef NORMAL
		normal_out = normal_in[vID];
	#endif

	#ifdef TEXTCOORD
		uv_out = uv_in[vID];
	#endif
    
    gl_Position = vec4(pos.x, -pos.y, 1.0, 1.0) + translate;
}
