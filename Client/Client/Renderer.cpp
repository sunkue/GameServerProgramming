#include "stdafx.h"
#include "Renderer.h"
#include "System.h"
#include "Game.h"
#include "Texture.h"

////////////////////////////////////////////////////////////////

Renderer::ScreenQuad::ScreenQuad()
{
	glGenVertexArrays(1, &quad_vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);

	struct QUAD
	{
		glm::vec2 pos;
		glm::vec2 tex;
	};

	QUAD quadv[] =
	{
		{ {-1,-1},{0,0} }
		,{{ 1, 1},{1,1} }
		,{{-1, 1},{0,1} }
		,{{-1,-1},{0,0} }
		,{{ 1,-1},{1,0} }
		,{{ 1, 1},{1,1} }
	};

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QUAD), (const GLvoid*)offsetof(QUAD, tex));

	glBindVertexArray(0);
}

void Renderer::ScreenQuad::draw_quad()
{
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

////////////////////////////////////////////////////////////////
void Renderer::ready_draw()
{
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw()
{
	ready_draw();

	bg_shader->use();
	bg_shader->set("u_texture", bg_tiles);
	bg_shader->set("u_start_with_dark", start_with_dark);
	bg_shader->set("u_focus_center", focus_center);
	bg_shader->set("u_raw_col", Game::get().get_n());
	ScreenQuad::get().draw_quad();

	obj_shader->use();
	obj_shader->set("u_texture", obj_tiles);
	obj_shader->set("u_start_with_dark", start_with_dark);
	obj_shader->set("u_focus_center", focus_center);
	obj_shader->set("u_raw_col", Game::get().get_n());

	for (const auto& obj : Game::get().get_objs())
	{
		obj_shader->set("u_type", int(obj->get_type()));
		obj_shader->set("u_position", obj->get_pos());
		ScreenQuad::get().draw_quad();
	}
}

void Renderer::init()
{
	glEnable(GL_BLEND);
	glViewport(10, 10, System::get().screen.width - 20, System::get().screen.height - 20);
	load_texture();
	load_shader();
}

void Renderer::load_shader()
{
	vector<string> VS; VS.emplace_back("./Resource/Shader/background.vert");
	vector<string> FS; FS.emplace_back("./Resource/Shader/background.frag");
	vector<string> GS;

	bg_shader = Shader::create(VS, FS, GS);

	VS.clear(); VS.emplace_back("./Resource/Shader/obj.vert");
	FS.clear(); FS.emplace_back("./Resource/Shader/obj.frag");
	GS.clear();
	obj_shader = Shader::create(VS, FS, GS);
}

void Renderer::load_texture()
{
	bg_tiles = Texture::create("background.png");
	obj_tiles = Texture::create("chess.png");
}
