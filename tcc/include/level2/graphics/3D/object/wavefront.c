#ifndef GRAPHICS_3D_OBJECT_WAVEFRONT
#define GRAPHICS_3D_OBJECT_WAVEFRONT


#include <file.c>
#include <data-structures/stack.c>
#include <system/graphics.c>
#include <graphics/image/TGA.c>


typedef struct
{
	Rational_Number x;
	Rational_Number y;
	Rational_Number z;
}
Wavefront_Object_Vertex;


typedef struct
{
	Rational_Number s;
	Rational_Number t;
}
Wavefront_Object_Texture_Coord;


typedef struct
{
	Number vertex_index;
	Number texture_coord_index;
	Number normal_index;
}
Wavefront_Object_Face_Index;


typedef struct
{
	Stack  name;
	Stack  points;
	Stack  lines;
	Stack  triangles;
	Stack  quads;
	Number material_index;
}
Wavefront_Object_Face;


typedef struct
{
	Stack  name;
	Number texture_id;
}
Wavefront_Object_Material;


typedef struct
{
	Stack vertices;
	Stack texture_coords;
	Stack normals;
	Stack faces;
	Stack materials;
}
Wavefront_Object;


static Boolean is_Wavefront_Object_space_character(char character)
{
	return character == ' ' || character == '\t';
}

static void read_Wavefront_Object_spaces(Reader* reader)
{
	while(!end_of_reader(reader) && is_Wavefront_Object_space_character(get_reader_byte(reader)))
		read_next_byte(reader);
}


Boolean read_Wavefront_Object_Face_Index(Reader* reader, Wavefront_Object_Face_Index* index)
{
	Byte character;

	read_Wavefront_Object_spaces(reader);
	character = get_reader_byte(reader);

	if(!is_number_character(character) && character != '-')
		return 0;

	index->vertex_index = read_Number(reader);
	index->texture_coord_index = 0;
	index->normal_index = 0;

	if(get_reader_byte(reader) == '/')
	{
		read_next_byte(reader);
		index->texture_coord_index = read_Number(reader);

		if(get_reader_byte(reader) == '/')
		{
			read_next_byte(reader);
			index->normal_index = read_Number(reader);
		}
	}

	return 1;
}


static void read_Wavefront_Object_Name(Reader* reader, Stack* name)
{
	Number16 character;
	
	read_Wavefront_Object_spaces(reader);
			
	while(!end_of_reader(reader))
	{
		character = get_reader_byte(reader);

		if(character == '\r' || character == '\n')
			break;

		add_bytes_in_stack(name, &character, sizeof(character));
		read_next_byte(reader);
	}
	character = 0;
	add_bytes_in_stack(name, &character, sizeof(character));
}


static void add_Wavefront_Object_relative_path(Stack* name, Number16* path_to_wavefront)
{
	Number last_slash_index;
	Number i;
	Number16 character;

	last_slash_index = 0;
	for(i = 0; path_to_wavefront[i]; ++i)
	{
		if(path_to_wavefront[i] == '/')
			last_slash_index = i + 1;
	}

	if(last_slash_index)
	{
		for(i = 0; i < last_slash_index; ++i)
		{
			character = path_to_wavefront[i];
			add_bytes_in_stack(name, &character, sizeof(character));
		}
	}
}


Boolean read_Wavefront_Object_Material_Library(Number16* file_name, Wavefront_Object* object, Number16* object_file_name)
{
	Reader                    reader;
	Wavefront_Object_Material material;

	initialize_file_reader(&reader, file_name);
	material.name.data = 0;
	material.texture_id = 0;

	while(!end_of_reader(&reader))
	{
		if(read_if(&reader, "map_Kd "))
		{
			if(!material.texture_id)
			{
				Stack texture_name;
				initialize_stack(&texture_name, 16);
				add_Wavefront_Object_relative_path(&texture_name, object_file_name);
				read_Wavefront_Object_Name(&reader, &texture_name);

				Reader texture_reader;
				initialize_file_reader(&texture_reader, texture_name.data);
				TGA_Image image;
				read_TGA_image(&texture_reader, &image);
				deinitialize_reader(&texture_reader);
				material.texture_id = bind_TGA_Image_to_texture(&image);
				deinitialize_TGA_image(&image);

				deinitialize_stack(&texture_name);
			}
		}
		else if(read_if(&reader, "newmtl "))
		{
			if(material.name.data)
			{
				add_bytes_in_stack(&object->materials, &material, sizeof(material));
				material.texture_id = 0;
			}

			initialize_stack(&material.name, 16);
			read_Wavefront_Object_Name(&reader, &material.name);
		}

		read_line(&reader);
	}

	if(material.name.data)
		add_bytes_in_stack(&object->materials, &material, sizeof(material));

	deinitialize_reader(&reader);

	return 1;
}


static Number find_Wavefront_Object_Material_index(Wavefront_Object* object, Stack* name)
{
	Wavefront_Object_Material* material;
	Number i;

	for(i = 0; i < object->materials.size; i += sizeof(Wavefront_Object_Material))
	{
		material = object->materials.data + i;

		if(!compare_bytes(material->name.data, material->name.size, name->data, name->size))
			return i + 1;
	}

	return 0;
}


Boolean read_Wavefront_Object(Number16* file_name, Wavefront_Object* object)
{
	Reader                         reader;
	Wavefront_Object_Vertex        vertex;
	Wavefront_Object_Texture_Coord texture_coord;
	Wavefront_Object_Face_Index    index1;
	Wavefront_Object_Face_Index    index2;
	Wavefront_Object_Face_Index    index3;
	Wavefront_Object_Face_Index    index4;
	Wavefront_Object_Face          face;

	initialize_file_reader(&reader, file_name);

	initialize_stack(&object->vertices, 128);
	initialize_stack(&object->texture_coords, 128);
	initialize_stack(&object->normals, 128);
	initialize_stack(&object->faces, 128);
	initialize_stack(&object->materials, 128);

	initialize_stack(&face.name, 16);
	initialize_stack(&face.points, 16);
	initialize_stack(&face.lines, 16);
	initialize_stack(&face.triangles, 128);
	initialize_stack(&face.quads, 128);

	while(!end_of_reader(&reader))
	{
		if(read_if(&reader, "v "))
		{
			read_Wavefront_Object_spaces(&reader);
			vertex.x = read_Rational_Number(&reader);

			read_Wavefront_Object_spaces(&reader);
			vertex.y = read_Rational_Number(&reader);

			read_Wavefront_Object_spaces(&reader);
			vertex.z = read_Rational_Number(&reader);

			add_bytes_in_stack(&object->vertices, &vertex, sizeof(vertex));
		}
		else if(read_if(&reader, "vt "))
		{
			read_Wavefront_Object_spaces(&reader);
			texture_coord.s = read_Rational_Number(&reader);

			read_Wavefront_Object_spaces(&reader);
			texture_coord.t = read_Rational_Number(&reader);

			add_bytes_in_stack(&object->texture_coords, &texture_coord, sizeof(texture_coord));
		}
		else if(read_if(&reader, "vn "))
		{
			read_Wavefront_Object_spaces(&reader);
			vertex.x = read_Rational_Number(&reader);

			read_Wavefront_Object_spaces(&reader);
			vertex.y = read_Rational_Number(&reader);

			read_Wavefront_Object_spaces(&reader);
			vertex.z = read_Rational_Number(&reader);

			add_bytes_in_stack(&object->normals, &vertex, sizeof(vertex));
		}
		else if(read_if(&reader, "f "))
		{
			read_Wavefront_Object_Face_Index(&reader, &index1);

			if(!read_Wavefront_Object_Face_Index(&reader, &index2))
			{
				add_bytes_in_stack(&face.points, &index1, sizeof(index1));
			}
			else if(!read_Wavefront_Object_Face_Index(&reader, &index3))
			{
				add_bytes_in_stack(&face.lines, &index1, sizeof(index1));
				add_bytes_in_stack(&face.lines, &index2, sizeof(index2));
			}
			else if(!read_Wavefront_Object_Face_Index(&reader, &index4))
			{
				add_bytes_in_stack(&face.triangles, &index1, sizeof(index1));
				add_bytes_in_stack(&face.triangles, &index2, sizeof(index2));
				add_bytes_in_stack(&face.triangles, &index3, sizeof(index3));
			}
			else
			{
				add_bytes_in_stack(&face.quads, &index1, sizeof(index1));
				add_bytes_in_stack(&face.quads, &index2, sizeof(index2));
				add_bytes_in_stack(&face.quads, &index3, sizeof(index3));
				add_bytes_in_stack(&face.quads, &index4, sizeof(index4));
			}
		}
		else if(read_if(&reader, "usemtl "))
		{
			Stack material_name;
			initialize_stack(&material_name, 16);
			read_Wavefront_Object_Name(&reader, &material_name);
			face.material_index = find_Wavefront_Object_Material_index(object, &material_name);
			deinitialize_stack(&material_name);
		}
		else if(read_if(&reader, "mtllib "))
		{
			Stack mtllib_file_name;
			initialize_stack(&mtllib_file_name, 16);
			add_Wavefront_Object_relative_path(&mtllib_file_name, file_name);
			read_Wavefront_Object_Name(&reader, &mtllib_file_name);	
			read_Wavefront_Object_Material_Library(mtllib_file_name.data, object, file_name);
			deinitialize_stack(&mtllib_file_name);
		}
		
		read_line(&reader);
	}

	add_bytes_in_stack(&object->faces, &face, sizeof(face));
	deinitialize_reader(&reader);

	return 1;

error:
	deinitialize_reader(&reader);
	return 0;
}


void deinitialize_Wavefront_Object(Wavefront_Object* object)
{

}


void draw_Wavefront_Object_Face_Index(Wavefront_Object* object, Wavefront_Object_Face_Index* index)
{
	Wavefront_Object_Vertex*        normal;
	Wavefront_Object_Vertex*        vertex;
	Wavefront_Object_Texture_Coord* texture_coord;

	if(index->normal_index > 0)
	{
		normal = object->normals.data + (index->normal_index - 1) * sizeof(Wavefront_Object_Vertex);
		glNormal3f(normal->x, normal->y, normal->z);
	}

	if(index->texture_coord_index > 0)
	{
		texture_coord = object->texture_coords.data + (index->texture_coord_index - 1) * sizeof(Wavefront_Object_Texture_Coord);
		glTexCoord2f(texture_coord->s, texture_coord->t);
	}

	if(index->vertex_index > 0)
	{
		vertex = object->vertices.data + (index->vertex_index - 1) * sizeof(Wavefront_Object_Vertex);
		glVertex3f(vertex->x, vertex->y, vertex->z);
	}
}


void draw_Wavefront_Object(Wavefront_Object* object)
{
	Number                       i;
	Number                       j;
	Wavefront_Object_Face*       face;
	Wavefront_Object_Face_Index* index;
	Wavefront_Object_Vertex*     vertex;
	Wavefront_Object_Material*   material;

	for(i = 0; i < object->faces.size; i += sizeof(Wavefront_Object_Face))
	{
		face = object->faces.data + i;

		if(face->material_index)
		{
			material = object->materials.data + (face->material_index - 1) * sizeof(Wavefront_Object_Material);

			if(material->texture_id)
				glBindTexture(GL_TEXTURE_2D, material->texture_id);
		}

		glBegin(GL_TRIANGLES);
		for(j = 0; j < face->triangles.size; j += sizeof(Wavefront_Object_Face_Index) * 3)
		{
			index = face->triangles.data + j;
			draw_Wavefront_Object_Face_Index(object, index);
			draw_Wavefront_Object_Face_Index(object, index + 1);
			draw_Wavefront_Object_Face_Index(object, index + 2);
		}
		glEnd();

		glBegin(GL_QUADS);
		for(j = 0; j < face->quads.size; j += sizeof(Wavefront_Object_Face_Index) * 4)
		{
			index = face->quads.data + j;
			draw_Wavefront_Object_Face_Index(object, index);
			draw_Wavefront_Object_Face_Index(object, index + 1);
			draw_Wavefront_Object_Face_Index(object, index + 2);
			draw_Wavefront_Object_Face_Index(object, index + 3);
		}
		glEnd();
	}
}


#endif//GRAPHICS_3D_OBJECT_WAVEFRONT

/*
#include <system/thread.c>

#include <audio/mixer.c>
#include <audio/WAV.c>

void main()
{
	Rational_Number32 angle;

	Graphics             graphics;
	Rational_Number32    aspect_ratio;

	initialize_graphics(&graphics, 1920, 1080);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-2.0, 2.0, -2.0, 2.0, -1.5, 1.5);
	//glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	//glMatrixMode(GL_MODELVIEW);

	aspect_ratio = 1080.0f / 1920.0f;
	glScalef(1.0 * aspect_ratio, 1.0, 1.0);

	glEnable(GL_LIGHTING);

	Rational_Number32 diffuse_color[]  = {1, 1, 1, 1};
	Rational_Number32 specular_color[] = {1, 1, 1, 1};
	Rational_Number32 ambient_color[]  = {0.9, 0.9, 0.9, 0};
	Rational_Number32 light_position[] = {0.0, 0.0, -10.0, 1};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);


	initialize_sound_mixer();
	WAV    wav;
	Reader reader;

	initialize_file_reader(&reader, L"a.wav");
	read_WAV(&wav, &reader);
	play_sound(&reader);


	Wavefront_Object object;
	read_Wavefront_Object(L"untitled.obj", &object);

	Wavefront_Object wave;
	read_Wavefront_Object(L"wave.obj", &wave);

	for(;;)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		
		glRotatef(angle, 0, 1, 0);
		angle += 1.1;
		//glTranslatef(10, 0, 0);
		draw_Wavefront_Object(&object);
		draw_Wavefront_Object(&wave);
		glPopMatrix();



		draw_graphics(&graphics);

		sleep_thread(10);
	}
}*/