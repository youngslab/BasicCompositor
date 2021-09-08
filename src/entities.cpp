

#include "entities.hpp"

namespace cx {

auto createSqure() -> Mesh {

  std::vector<float> vertices = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f	// top left
  };

  std::vector<uint32_t> indices = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  return cx::Mesh(vertices, indices,
		  {{GL_FLOAT, 3, GL_FALSE},
		   {GL_FLOAT, 3, GL_FALSE},
		   {GL_FLOAT, 2, GL_FALSE}});
}

auto createTextureMaterial(gl::Texture t) -> Material {
  const char *vShaderStr = "#version 300 es\n"
			   "layout(location = 0) in vec3 aPos;\n"
			   "layout(location = 2) in vec2 aTexCoord;\n"
			   "out vec2 TexCoord;\n"
			   "void main() {\n"
			   "gl_Position = vec4(aPos, 1.0);\n"
			   "TexCoord = aTexCoord;\n"
			   "}\n";

  const char *fShaderStr = "#version 300 es\n"
			   "precision mediump float;\n"
			   "out vec4 FragColor;\n"
			   "in vec2 TexCoord;\n"
			   "uniform sampler2D tex;\n"
			   "void\n"
			   "main() {\n"
			   "FragColor = texture(tex, TexCoord);\n "
			   "}\n";

  auto p = gl::createProgram_(vShaderStr, fShaderStr);

  glUseProgram(p);
  glUniform1i(glGetUniformLocation(p, "tex"), 0);

  return cx::Material(p, {t});
}
} // namespace cx
