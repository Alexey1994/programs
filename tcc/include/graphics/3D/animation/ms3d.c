#ifndef GRAPHICS_3D_ANIMATION_INCLUDED
#define GRAPHICS_3D_ANIMATION_INCLUDED


#include <file.c>
#include <system/graphics.c>


typedef struct
{
	Number8           flags;
	Rational_Number32 coords[3];
	Number8           bone_id;
	Number8           reference_count;
}
MS3D_Model_Vertex;


typedef struct
{
	Number16          flags;
	Number16          vertex_indices[3];
	Rational_Number32 normals[9];
	Rational_Number32 texture_s[3];
	Rational_Number32 texture_t[3];
	Number8           smoothing_group;
	Number8           group_index;
}
MS3D_Model_Triangle;


typedef struct
{
	Number               number_of_vertices;
	MS3D_Model_Vertex*   vertices;

	Number               number_of_triangles;
	MS3D_Model_Triangle* triangles;

}
MS3D_Model;


Boolean read_MS3D(MS3D_Model* model, Reader* reader)
{
	Number i;

	if(!read_if(reader, "MS3D000000"))
		goto error;

	if(read_binary_Number32(reader) != 4) //version
		goto error;

	model->number_of_vertices = read_binary_Number16(reader);
	model->vertices = allocate_memory(model->number_of_vertices * sizeof(MS3D_Model_Vertex));
	for(i = 0; i < model->number_of_vertices; ++i)
	{
		model->vertices[i].flags = read_binary_Number8(reader);
		model->vertices[i].coords[0] = read_binary_Rational_Number32(reader);
		model->vertices[i].coords[1] = read_binary_Rational_Number32(reader);
		model->vertices[i].coords[2] = read_binary_Rational_Number32(reader);
		model->vertices[i].bone_id = read_binary_Number8(reader);
		model->vertices[i].reference_count = read_binary_Number8(reader);
	}

	model->number_of_triangles = read_binary_Number16(reader);
	model->triangles = allocate_memory(model->number_of_triangles * sizeof(MS3D_Model_Triangle));
	for(i = 0; i < model->number_of_triangles; ++i)
	{
		model->triangles[i].flags = read_binary_Number16(reader);
		model->triangles[i].vertex_indices[0] = read_binary_Number16(reader);
		model->triangles[i].vertex_indices[1] = read_binary_Number16(reader);
		model->triangles[i].vertex_indices[2] = read_binary_Number16(reader);
		model->triangles[i].normals[0] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[1] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[2] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[3] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[4] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[5] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[6] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[7] = read_binary_Rational_Number32(reader);
		model->triangles[i].normals[8] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_s[0] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_s[1] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_s[2] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_t[0] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_t[1] = read_binary_Rational_Number32(reader);
		model->triangles[i].texture_t[2] = read_binary_Rational_Number32(reader);
		model->triangles[i].smoothing_group = read_binary_Number8(reader);
		model->triangles[i].group_index = read_binary_Number8(reader);
	}

	return 1;

error:
	printf("error read ms3d model\n");
	return 0;
}


void draw_MS3D_model(MS3D_Model* model)
{
	Number               i;
	MS3D_Model_Vertex*   vertex;

	glBegin(GL_TRIANGLES);
	for(i = 0; i < model->number_of_triangles; ++i)
	{
		vertex = model->vertices + model->triangles[i].vertex_indices[0];
		glVertex3f(vertex->coords[0], vertex->coords[1], vertex->coords[2]);

		vertex = model->vertices + model->triangles[i].vertex_indices[1];
		glVertex3f(vertex->coords[0], vertex->coords[1], vertex->coords[2]);

		vertex = model->vertices + model->triangles[i].vertex_indices[2];
		glVertex3f(vertex->coords[0], vertex->coords[1], vertex->coords[2]);
	}
	glEnd();
}


#endif//GRAPHICS_3D_ANIMATION_INCLUDED


void main()
{
	Reader     reader;
	MS3D_Model model;
	
	initialize_file_reader(&reader, L"cylinder.ms3d");
	read_MS3D(&model, &reader);
	deinitialize_reader(&reader);

	Graphics             graphics;
	Rational_Number32    aspect_ratio;

	initialize_graphics(&graphics, 1920, 1080);

	aspect_ratio = 1080.0f / 1920.0f;
	glScalef(0.15 * aspect_ratio, 0.15, 0.15);

	for(;;)
	{
		draw_MS3D_model(&model);
		draw_graphics(&graphics);	
	}
}