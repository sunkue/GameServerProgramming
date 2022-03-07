#include "stdafx.h"
#include "Shader.h"

Shader::Shader(vector<string>& filenameVS, vector<string>& filenameFS, vector<string>& filenameGS)
	: shader_id_{ compile_shader(filenameVS, filenameFS, filenameGS) } {}

/// //////////////////////////////////////////////

bool read_file(string_view filename, std::string& target)
{
	std::ifstream file(filename.data());
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target.append(line.c_str());
		target.append("\n");
	}
	return true;
}

/// //////////////////////////////////////////////

void Shader::add_shader(GLuint shader_program, const char* raw_shader, GLenum shader_type)
{
	GLuint ShaderObj = glCreateShader(shader_type);
	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", shader_type);
	}

	const GLchar* p[1] = { raw_shader };
	GLint Lengths[1] = {(GLint)strlen(raw_shader)};

	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, InfoLog);
		printf("%s \n", raw_shader);
	}

	glAttachShader(shader_program, ShaderObj);
	glDeleteShader(ShaderObj);
}

GLuint Shader::compile_shader(vector<string>& filenameVS, vector<string>& filenameFS, vector<string>& filenameGS)
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs, gs;

	for (auto& vertex : filenameVS)
	{
		std::string temp;
		if (!read_file(vertex, temp)) {
			printf("Error compiling vertex shader\n");
			return -1;
		};
		vs += temp;
	}

	for (auto& fragment : filenameFS)
	{
		std::string temp;
		if (!read_file(fragment, temp)) {
			printf("Error compiling fragment shader\n");
			return -1;
		};
		fs += temp;
	}

	for (auto& geometry : filenameGS)
	{
		std::string temp;
		if (!read_file(geometry, temp)) {
			printf("Error compiling geometry shader\n");
			return -1;
		};
		gs += temp;
	}
	//	cout << vs<<"==\n\n\n";
	//	cout << fs<<"==\n\n\n";
	//	cout << gs<<"==\n\n\n";

	add_shader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	add_shader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);
	if (filenameGS.empty())
	{
		filenameGS.emplace_back("none_geometry");
	}
	else
	{
		add_shader(ShaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);
	}

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << filenameVS.back() << ", " << filenameFS.back() << ", " << filenameGS.back() << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << filenameVS.back() << ", " << filenameFS.back() << ", " << filenameGS.back() << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(0);
	std::cerr << filenameVS.back() << ", " << filenameFS.back() << ", " << filenameGS.back() << " Shader compiling is done.\n";

	return ShaderProgram;
}

/// //////////////////////////////////////////////

void Shader::use()const
{
	glUseProgram(shader_id_);
}

