#pragma once

using ShaderPtr = shared_ptr<class Shader>;
class Shader
{
public:

	void use()const;

	template<class T> void set(const string& uniform_var_name, const T& value)const;

	GET(shader_id);

	CREATE_SHARED(ShaderPtr, Shader);

	static ShaderPtr basic()
	{
		vector<string> VS;
		vector<string> FS;
		vector<string> GS;
		VS.clear(); VS.emplace_back("./Shader/test_vertex.glsl"sv);
		FS.clear(); FS.emplace_back("./Shader/default_fragment.glsl"sv);
		GS.clear(); 
		static ShaderPtr basic = Shader::create(VS, FS, GS);
		return basic;
	}
	
private:
	Shader(vector<string>& filenameVS, vector<string>& filenameFS, vector<string>& filenameGS);
	
private:
	void add_shader(GLuint ShaderProgram, const char* pShaderText, GLenum shader_type);
	GLuint compile_shader(vector<string>& filenameVS, vector<string>& filenameFS, vector<string>& filenameGS);

private:
	GLuint shader_id_;
};


#include "Shader.hpp"