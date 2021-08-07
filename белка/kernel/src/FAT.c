#ifndef FAT_INCLUDED
#define FAT_INCLUDED


#include <memory.c>


typedef enum
{
	FAT_READ_ONLY = 0x01,
	FAT_HIDDEN    = 0x02,
	FAT_SYSTEM    = 0x04,
	FAT_VOLUME_ID = 0x08,
	FAT_DIRECTORY = 0x10,
	FAT_ARCHIVE   = 0x20,
	FAT_LONG_NAME = 0x0F
}
FAT_Data_Attribute;


typedef struct
{
	Byte     name[11];
	Byte     attributes;
	Byte     zero;
	Number8  milliseconds_creation;
	Number16 time_created;
	Number16 date_created;
	Number16 last_access_date;
	Number16 cluster_number_high;
	Number16 time_last_write;
	Number16 date_last_write;
	Number16 cluster_number_low;
	Number32 file_size;
}
FAT_Data;


typedef struct
{
	Byte     os_name[8];                 //имя того кто форматировал
	Number16 bytes_in_sector;            //512, 1024, 2048 или 4096
	Number8  sectors_in_cluster;         //1, 2, 4, 8, 16, 32, 64 или 128
	Number16 number_of_reserved_sectors; //любое число кроме 0
	Number8  number_of_FAT_tables;       //2 рекомендуется, 1 приемлемо
	Number32 zeros;
	Number8  type;                       //0xF8 - не перемещаемый
	Number16 zeros2;
	Number16 sectors_per_track;          //секторов в дорожке - 63
	Number16 number_of_heads;            //головок - 255
	Number32 number_of_hidden_sectors;   //0 если нет разделов
	Number32 number_of_sectors;          //не 0

	//FAT32 часть
	Number32 number_of_sectors_in_FAT_table;
	Number16 flags;                           //биты 0-3 - номер активного FAT, биты 4-6 - зарезервировано, бит 7 - 1 если один FAT активен, биты 8-15 - зарезервировано. Рекомендуется 0
	Number16 version;                         //0
	Number32 root_directory_cluster;          //обычно 2 (т.е. самый первый кластер data раздела)
	Number16 FSInfo_sector_number;            //обычно 1 (0 или 0xFFFF - нет FSInfo)
	Number16 backup_sector_number;            //0 или 6
	Byte     zeros3[12];
	Number8  drive_number;                    //0x00 или 0x80
	Byte     zeros4;
	Number8  signature;                       //0x29
	Number32 volume_serial_number;            //генерируется как время создания
	Byte     volume_label[11];                //имя раздела
	Byte     file_system_name[8];             //FAT32 и 3 пробела
}
FAT32;


typedef struct
{
	Byte     os_name[8];                 //имя того кто форматировал
	Number16 bytes_in_sector;            //512, 1024, 2048 или 4096
	Number8  sectors_in_cluster;         //1, 2, 4, 8, 16, 32, 64 или 128
	Number16 number_of_reserved_sectors; //любое число кроме 0
	Number8  number_of_FAT_tables;       //2 рекомендуется, 1 приемлемо
	Number32 zeros;
	Number8  type;                       //0xF8 - не перемещаемый
	Number16 zeros2;
	Number16 sectors_per_track;          //секторов в дорожке - 63
	Number16 number_of_heads;            //головок - 255
	Number32 number_of_hidden_sectors;   //0 если нет разделов
	Number32 number_of_sectors;          //не 0

	//FAT32 часть
	Number32 number_of_sectors_in_FAT_table;
	Number16 flags;                           //биты 0-3 - номер активного FAT, биты 4-6 - зарезервировано, бит 7 - 1 если один FAT активен, биты 8-15 - зарезервировано. Рекомендуется 0
	Number16 version;                         //0
	Number32 root_directory_cluster;          //обычно 2 (т.е. самый первый кластер data раздела)
	Number16 FSInfo_sector_number;            //обычно 1 (0 или 0xFFFF - нет FSInfo)
	Number16 backup_sector_number;            //0 или 6
	Byte     zeros3[12];
	Number8  drive_number;                    //0x00 или 0x80
	Byte     zeros4;
	Number8  signature;                       //0x29
	Number32 volume_serial_number;            //генерируется как время создания
	Byte     volume_label[11];                //имя раздела
	Byte     file_system_name[8];             //FAT32 и 3 пробела
}
FAT32;


typedef struct
{
	Number32 FAT_start_sector;
	Number32 data_start_sector;
	Number32 sectors_in_cluster;
	Number32 number_of_clusters;

	FAT_Data current_directory;

	Number32 current_FAT_sector;
	Number32 FAT[128];

	Byte* storage;
	Boolean (*read_sector)(Byte* storage, Number32 sector_number, Byte* sector);
	Boolean (*write_sector)(Byte* storage, Number32 sector_number, Byte* sector);
}
FAT_File_System;


void read_FAT_sector(FAT_File_System* file_system, Number32 cluster_number, Byte* sector, Number32 offset)
{
	file_system->read_sector(file_system->storage, file_system->data_start_sector + (cluster_number - 2) * file_system->sectors_in_cluster + offset, sector);
}


void write_FAT_sector(FAT_File_System* file_system, Number32 cluster_number, Byte* sector, Number32 offset)
{
	file_system->write_sector(file_system->storage, file_system->data_start_sector + (cluster_number - 2) * file_system->sectors_in_cluster + offset, sector);
}


void open_FAT_File_System(
	FAT_File_System* file_system,
	Byte* storage,
	Boolean (*read_sector)(Byte* storage, Number32 sector_number, Byte* sector),
	Boolean (*write_sector)(Byte* storage, Number32 sector_number, Byte* sector)
)
{
	Byte   sector[512];
	FAT32* fat32;

	file_system->storage      = storage;
	file_system->read_sector  = read_sector;
	file_system->write_sector = write_sector;

	file_system->read_sector(file_system->storage, 1, sector);
	fat32 = sector + 3;

	file_system->FAT_start_sector   = fat32->number_of_hidden_sectors + fat32->number_of_reserved_sectors;
	file_system->data_start_sector  = file_system->FAT_start_sector + fat32->number_of_FAT_tables * fat32->number_of_sectors_in_FAT_table;
	file_system->sectors_in_cluster = fat32->sectors_in_cluster;
	file_system->number_of_clusters = fat32->number_of_sectors / fat32->sectors_in_cluster;

	clear_bytes(&file_system->current_directory, sizeof(file_system->current_directory));
	file_system->current_directory.cluster_number_low = fat32->root_directory_cluster;
	file_system->current_directory.cluster_number_high = fat32->root_directory_cluster >> 16;

	file_system->current_FAT_sector = 0;
	file_system->read_sector(file_system->storage, file_system->FAT_start_sector + file_system->current_FAT_sector, file_system->FAT);
}


Number32 next_FAT_cluster(FAT_File_System* file_system, Number32 current_cluster)
{
	if(current_cluster / 128 != file_system->current_FAT_sector)
	{
		file_system->current_FAT_sector = current_cluster / 128;
		file_system->read_sector(file_system->storage, file_system->FAT_start_sector + file_system->current_FAT_sector, file_system->FAT);
	}

	return file_system->FAT[current_cluster % 128];
}


Number32 allocate_FAT_cluster(FAT_File_System* file_system)
{
	Number32 free_cluster;
	Number   i;

	free_cluster = 0;

	for(
		file_system->current_FAT_sector = file_system->FAT_start_sector;
		free_cluster < file_system->number_of_clusters;
		++file_system->current_FAT_sector
	)
	{
		file_system->read_sector(file_system->storage, file_system->current_FAT_sector, file_system->FAT);

		for(i = 0; i < 128; ++i)
		{
			if(!file_system->FAT[i])
				goto free_cluster_finded;

			++free_cluster;
		}
	}

	log_error("no free space");
	return 0;

free_cluster_finded:

	file_system->FAT[i] = 0x0FFFFFFF;
	file_system->write_sector(file_system->storage, file_system->current_FAT_sector, file_system->FAT);
	
	return free_cluster;
}


Number32 allocate_next_FAT_cluster(FAT_File_System* file_system, Number32 current_cluster)
{
	Number32 allocated_cluster;

	allocated_cluster = allocate_FAT_cluster(file_system);

	if(!allocated_cluster)
		goto error;

	if(current_cluster / 128 != file_system->current_FAT_sector)
	{
		file_system->current_FAT_sector = current_cluster / 128;
		file_system->read_sector(file_system->storage, file_system->FAT_start_sector + file_system->current_FAT_sector, file_system->FAT);
	}

	file_system->FAT[current_cluster % 128] = allocated_cluster;
	file_system->write_sector(file_system->storage, file_system->FAT_start_sector + file_system->current_FAT_sector, file_system->FAT);

	return allocated_cluster;

error:
	return 0;
}


void enum_FAT_data(
	FAT_File_System* file_system,
	Number32 data_cluster,
	Boolean(*on_data)(FAT_File_System* file_system, FAT_Data* data, Number16* name, Number8 data_offset, Byte* context),
	Byte* context
)
{
	Byte      sector[512];
	Number16  name[256];
	FAT_Data* data;
	Number    cluster_offset;
	Number    data_offset;
	Number    i;

	do
	{
		for(cluster_offset = 0; cluster_offset < file_system->sectors_in_cluster; ++cluster_offset)
		{
			read_FAT_sector(file_system, data_cluster, sector, cluster_offset);
			data = sector;

			for(data_offset = 0; data_offset < 512 / sizeof(FAT_Data); ++data_offset)
			{
				if(!data->name[0])
					return;

				for(i = 0; i < 11; ++i)
					name[i] = data->name[i];

				for(i = 11; i && name[i - 1] == ' '; --i)
					name[i - 1] = '\0';

				if(!on_data(file_system, data, name, data_offset, context))
					return;

				++data;
			}
		}

		data_cluster = next_FAT_cluster(file_system, data_cluster);
	}
	while(data_cluster != 0x0FFFFFFF);
}


static Boolean print_FAT_data(FAT_File_System* file_system, FAT_Data* data, Number16* name)
{
	print("wc", name, '\n');
	return 1;
}

void list_FAT_directory(FAT_File_System* file_system)
{
	enum_FAT_data(
		file_system,
		file_system->current_directory.cluster_number_low + (file_system->current_directory.cluster_number_high << 16),
		&print_FAT_data,
		0
	);
}


typedef struct
{
	Number16* data_name;
	Boolean   finded;
	FAT_Data* finded_data;
}
FAT_Find_Context;

static Boolean on_FAT_data_find(FAT_File_System* file_system, FAT_Data* data, Number16* name, Number8 data_offset, FAT_Find_Context* context)
{
	if(!compare_null_terminated_words(context->data_name, name))
	{
		context->finded = 1;
		copy_bytes(context->finded_data, data, sizeof(*data));
		return 0;
	}

	return 1;
}

static Boolean open_FAT_data(FAT_File_System* file_system, Number16* data_name, Number32 data_cluster, FAT_Data* data)
{
	FAT_Find_Context context;

	context.data_name   = data_name;
	context.finded      = 0;
	context.finded_data = data;
	enum_FAT_data(file_system, data_cluster, &on_FAT_data_find, &context);

	return context.finded;
}


Boolean open_FAT_directory(FAT_File_System* file_system, Number16* directory_name)
{
	FAT_Data directory;

	if(!open_FAT_data(
		file_system,
		directory_name,
		file_system->current_directory.cluster_number_low + (file_system->current_directory.cluster_number_high << 16),
		&directory
	))
		goto error;

	if(!(directory.attributes & FAT_DIRECTORY))
		goto error;

	copy_bytes(&file_system->current_directory, &directory, sizeof(file_system->current_directory));

	return 1;

error:
	return 0;
}


Boolean open_FAT_file(FAT_File_System* file_system, Number16* directory_name, FAT_Data* file)
{
	FAT_Data directory;

	if(!open_FAT_data(
		file_system,
		directory_name,
		file_system->current_directory.cluster_number_low + (file_system->current_directory.cluster_number_high << 16),
		file
	))
		goto error;

	if(file->attributes & FAT_DIRECTORY)
		goto error;

	return 1;

error:
	return 0;
}


Boolean allocate_FAT_data(FAT_File_System* file_system, Number16* name, FAT_Data_Attribute attributes, Number32 cluster_number, Number32 file_size)
{
	Byte      sector[512];
	FAT_Data* data;
	Number32  data_cluster;
	Number32  next_data_cluster;
	Number    cluster_offset;
	Number    data_offset;
	Number    i;

	data_cluster = file_system->current_directory.cluster_number_low + (file_system->current_directory.cluster_number_high << 16);

	FAT_Data directory;
	if(open_FAT_data(file_system, name, data_cluster, &directory))
	{
		print("ws", name ," exists\n");
		goto error;
	}
	
	for(;;)
	{
		for(cluster_offset = 0; cluster_offset < file_system->sectors_in_cluster; ++cluster_offset)
		{
			read_FAT_sector(file_system, data_cluster, sector, cluster_offset);
			data = sector;

			for(data_offset = 0; data_offset < 512 / sizeof(FAT_Data); ++data_offset)
			{
				if(!data->name[0])
					goto finded_new_data_place;

				++data;
			}
		}

		next_data_cluster = next_FAT_cluster(file_system, data_cluster);

		if(next_data_cluster == 0x0FFFFFFF)
			break;

		data_cluster = next_data_cluster;
	}

	cluster_offset = 0;
	data_cluster = allocate_next_FAT_cluster(file_system, data_cluster);

	if(!data_cluster)
		goto error;

	clear_bytes(sector, 512);
	data = sector;

finded_new_data_place:
	for(i = 0; i < 11; ++i)
	{
		if(*name)
		{
			data->name[i] = *name;
			++name;
		}
		else
			data->name[i] = ' ';
	}

	data->attributes          = attributes;
	data->cluster_number_low  = cluster_number;
	data->cluster_number_high = cluster_number >> 16;
	data->file_size           = file_size;

	write_FAT_sector(file_system, data_cluster, sector, cluster_offset);

	return 1;

error:
	return 0;
}


Boolean update_FAT_data(FAT_File_System* file_system, FAT_Data* data)
{

}


Boolean create_FAT_directory(FAT_File_System* file_system, Number16* name)
{
	Number32 cluster_number;

	cluster_number = allocate_FAT_cluster(file_system);

	if(!cluster_number)
		goto error;

	if(!allocate_FAT_data(file_system, name, FAT_DIRECTORY, cluster_number, 0))
		goto error;

	return 1;

error:
	return 0;
}


Boolean create_FAT_file(FAT_File_System* file_system, Number16* name, Byte* bytes, Number32 size_of_bytes)
{
	Number32 number_of_clusters;
	Number32 cluster_number;
	Number32 i;

	number_of_clusters = size_of_bytes / (512 * file_system->sectors_in_cluster);
	if(size_of_bytes % (512 * file_system->sectors_in_cluster))
		++number_of_clusters;

	cluster_number = allocate_FAT_cluster(file_system);

	if(!cluster_number)
		goto error;

	if(!allocate_FAT_data(file_system, name, 0, cluster_number, size_of_bytes))
		goto error;

	if(number_of_clusters)
	{
		for(;;)
		{
			for(i = 0; i < file_system->sectors_in_cluster; ++i)
			{
				if(size_of_bytes > 512)
				{
					write_FAT_sector(file_system, cluster_number, bytes, i);
					bytes += 512;
					size_of_bytes -= 512;
				}
				else
				{
					if(size_of_bytes)
						write_FAT_sector(file_system, cluster_number, bytes, i); //выход за пределы выделенного массива!

					size_of_bytes = 0;
				}
			}

			--number_of_clusters;

			if(!number_of_clusters)
				break;

			cluster_number = allocate_next_FAT_cluster(file_system, cluster_number);

			if(!cluster_number)
				goto error;
		}
	}

	return 1;

error:
	return 0;
}


#endif//FAT_INCLUDED