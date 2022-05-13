#pragma once

using ShaderPtr = shared_ptr<class Shader>;
class Shader
{
public:

	void Use()const;

	template<class T> void Set(const string& uniform_var_name, const T& value)const;

	GET(shader_id);

	CREATE_SHARED(Shader);

	static ShaderPtr Basic()
	{
		vector<string> VS;
		vector<string> FS;
		vector<string> GS;
		VS.clear(); VS.emplace_back("./Shader/test_vertex.glsl");
		FS.clear(); FS.emplace_back("./Shader/default_fragment.glsl");
		GS.clear(); 
		static ShaderPtr Basic = Shader::Create(VS, FS, GS);
		return Basic;
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