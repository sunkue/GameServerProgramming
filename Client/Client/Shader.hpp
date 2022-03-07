#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "Texture.h"
#include <array>

/////////////////////////////////////////

#define TEMPLATE_SHADER_SET(type) template<> inline void Shader::set<type>(const string& uniform_var_name, const type& value)const
#define UNIFORM_LOCATION glGetUniformLocation(shader_id_, uniform_var_name.data())
#define UNIFORM_LOCATION_STRUCT(mem) glGetUniformLocation(shader_id_, uniform_var_name.data() + #mem)

/////////////////////////////////////////

template<class T>
inline void Shader::set(const string& uniform_var, const T& value)const
{
	static_assert(sizeof(T) == 0, "Shader::set T&, come to Shader.hpp and specialize template for type.");
}

/////////////////////////////////////////

TEMPLATE_SHADER_SET(bool)
{
	glUniform1i(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(int)
{
	glUniform1i(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(float)
{
	glUniform1f(UNIFORM_LOCATION, value);
}

TEMPLATE_SHADER_SET(glm::mat4)
{
	glUniformMatrix4fv(UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(value));
}

using v6mat4 = std::array<glm::mat4, 6>;
TEMPLATE_SHADER_SET(v6mat4)
{
	glUniformMatrix4fv(UNIFORM_LOCATION, 6, GL_FALSE, glm::value_ptr(value.front()));
}

TEMPLATE_SHADER_SET(glm::vec2)
{
	glUniform2fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(glm::vec3)
{
	glUniform3fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(glm::vec4)
{
	glUniform4fv(UNIFORM_LOCATION, 1, glm::value_ptr(value));
}

TEMPLATE_SHADER_SET(TexturePtr)
{
	glUniform1i(UNIFORM_LOCATION, value->id);
	glActiveTexture(GL_TEXTURE0 + value->id);
	glBindTexture(GL_TEXTURE_2D, value->id);
}

#define SET_LIGHT_POWER()								\
set(uniform_var_name + ".power.ambient", value->ambient);		\
set(uniform_var_name + ".power.diffuse", value->diffuse);		\
set(uniform_var_name + ".power.specular", value->specular);

/////////////////////////////////////////


/////////////////////////////////////////

#undef TEMPLATE_SHADER_SET
#undef UNIFORM_LOCATION
#undef SET_LIGHT_POWER