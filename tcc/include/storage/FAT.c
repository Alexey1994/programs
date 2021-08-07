#ifndef FAT_STORAGE
#define FAT_STORAGE


#include <file.c>
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
	Byte     os_name[8];                   //имя того кто форматировал
	Number16 bytes_in_sector;              //512, 1024, 2048 или 4096
	Number8  sectors_in_cluster;           //1, 2, 4, 8, 16, 32, 64 или 128
	Number16 number_of_reserved_sectors;   //любое число кроме 0, выставляю 1, т.е. размер загрузочного сектора, затем FAT
	Number8  number_of_FAT_tables;         //2 рекомендуется, 1 приемлемо
	Number16 number_of_directories;        //количество 32 байтных директорий, 512 рекомендуется
	Number16 number_of_sectors_16;         //если 0 то берётся из number_of_sectors_32(для больше чем 0x10000 секторов)
	Number8  type;                         //0xF8 - не перемещаемый
	Number16 number_of_sectors_for_one_FAT;
	Number16 sectors_per_track;            //секторов в дорожке - 63
	Number16 number_of_heads;              //головок - 255
	Number32 number_of_hidden_sectors;     //0 если нет разделов
	Number32 number_of_sectors_32;         //если number_of_sectors_16 не 0, то должно быть 0

	//FAT12 часть
	Number8  drive_number;                    //0x00 или 0x80
	Byte     zero;
	Number8  signature;                       //0x29
	Number32 volume_serial_number;            //генерируется как время создания
	Byte     volume_label[11];                //имя раздела
	Byte     file_system_name[8];             //FAT12 и 3 пробела
}
FAT12;


Byte bootcode_FAT12[] = {
	0x33, 0xC9, 0x8E, 0xD1, 0xBC, 0xF0, 0x7B, 0x8E, 0xD9, 0xB8, 0x00, 0x20,
	0x8E, 0xC0, 0xFC, 0xBD, 0x00, 0x7C, 0x38, 0x4E, 0x24, 0x7D, 0x24, 0x8B,
	0xC1, 0x99, 0xE8, 0x3C, 0x01, 0x72, 0x1C, 0x83, 0xEB, 0x3A, 0x66, 0xA1,
	0x1C, 0x7C, 0x26, 0x66, 0x3B, 0x07, 0x26, 0x8A, 0x57, 0xFC, 0x75, 0x06,
	0x80, 0xCA, 0x02, 0x88, 0x56, 0x02, 0x80, 0xC3, 0x10, 0x73, 0xEB, 0x33,
	0xC9, 0x8A, 0x46, 0x10, 0x98, 0xF7, 0x66, 0x16, 0x03, 0x46, 0x1C, 0x13,
	0x56, 0x1E, 0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11, 0x60, 0x89,
	0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8, 0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E,
	0x0B, 0x03, 0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11, 0x4E, 0xFE,
	0x61, 0xBF, 0x00, 0x00, 0xE8, 0xE6, 0x00, 0x72, 0x39, 0x26, 0x38, 0x2D,
	0x74, 0x17, 0x60, 0xB1, 0x0B, 0xBE, 0xA1, 0x7D, 0xF3, 0xA6, 0x61, 0x74,
	0x32, 0x4E, 0x74, 0x09, 0x83, 0xC7, 0x20, 0x3B, 0xFB, 0x72, 0xE6, 0xEB,
	0xDC, 0xA0, 0xFB, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x98, 0x40, 0x74,
	0x0C, 0x48, 0x74, 0x13, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB,
	0xEF, 0xA0, 0xFD, 0x7D, 0xEB, 0xE6, 0xA0, 0xFC, 0x7D, 0xEB, 0xE1, 0xCD,
	0x16, 0xCD, 0x19, 0x26, 0x8B, 0x55, 0x1A, 0x52, 0xB0, 0x01, 0xBB, 0x00,
	0x00, 0xE8, 0x3B, 0x00, 0x72, 0xE8, 0x5B, 0x8A, 0x56, 0x24, 0xBE, 0x0B,
	0x7C, 0x8B, 0xFC, 0xC7, 0x46, 0xF0, 0x3D, 0x7D, 0xC7, 0x46, 0xF4, 0x29,
	0x7D, 0x8C, 0xD9, 0x89, 0x4E, 0xF2, 0x89, 0x4E, 0xF6, 0xC6, 0x06, 0x96,
	0x7D, 0xCB, 0xEA, 0x03, 0x00, 0x00, 0x20, 0x0F, 0xB6, 0xC8, 0x66, 0x8B,
	0x46, 0xF8, 0x66, 0x03, 0x46, 0x1C, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA,
	0x10, 0xEB, 0x5E, 0x0F, 0xB6, 0xC8, 0x4A, 0x4A, 0x8A, 0x46, 0x0D, 0x32,
	0xE4, 0xF7, 0xE2, 0x03, 0x46, 0xFC, 0x13, 0x56, 0xFE, 0xEB, 0x4A, 0x52,
	0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A, 0x10, 0x91, 0x8B, 0x46, 0x18, 0x96,
	0x92, 0x33, 0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42, 0x87, 0xCA, 0xF7,
	0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8, 0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8,
	0x01, 0x02, 0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42, 0x8B, 0xF4,
	0x8A, 0x56, 0x24, 0xCD, 0x13, 0x61, 0x61, 0x72, 0x0B, 0x40, 0x75, 0x01,
	0x42, 0x03, 0x5E, 0x0B, 0x49, 0x75, 0x06, 0xF8, 0xC3, 0x41, 0xBB, 0x00,
	0x00, 0x60, 0x66, 0x6A, 0x00, 0xEB, 0xB0, 0x4E, 0x54, 0x4C, 0x44, 0x52,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x4E, 0x54, 0x4C, 0x44,
	0x52, 0x20, 0x69, 0x73, 0x20, 0x6D, 0x69, 0x73, 0x73, 0x69, 0x6E, 0x67,
	0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73, 0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F,
	0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x61, 0x6E,
	0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73,
	0x74, 0x61, 0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xAC, 0xBF, 0xCC, 0x55, 0xAA
};


void format_FAT12(File storage_file, Number64 storage_file_size)
{
	Byte   sector[512];
	FAT12* fat12;

	clear_bytes(sector, 512);

	sector[0] = 0xEB;
	sector[1] = 0x3C; //jmp 62
	sector[2] = 0x90; //nop

	fat12 = sector + 3;
	copy_bytes(fat12->os_name, "MSDOS5.0", 8);
	fat12->bytes_in_sector               = 512;
	fat12->sectors_in_cluster            = 1;
	fat12->number_of_reserved_sectors    = 1;
	fat12->number_of_FAT_tables          = 1;
	fat12->number_of_directories         = 512;
	fat12->number_of_sectors_16          = storage_file_size / 512 - 1;
	fat12->type                          = 0xF8;
	fat12->number_of_sectors_for_one_FAT = storage_file_size / (315 * 512);
	fat12->sectors_per_track             = 63;
	fat12->number_of_heads               = 255;
	fat12->number_of_hidden_sectors      = 1;
	fat12->number_of_sectors_32          = 0;

	fat12->drive_number         = 0x80;
	fat12->signature            = 0x29;
	fat12->volume_serial_number = 2000000000;
	copy_bytes(fat12->volume_label, "NO NAME    ", 11);
	copy_bytes(fat12->file_system_name, "FAT12   ", 8);

	copy_bytes(sector + 62, bootcode_FAT12, sizeof(bootcode_FAT12));

	sector[510] = 0x55;
	sector[511] = 0xAA;

	write_in_file(storage_file, 512, sector, 512);


	Byte* fat = sector;

	clear_bytes(sector, 512);
	fat[0] = 0xF8;//fat12->type;
	fat[1] = 0xFF;
	fat[2] = 0xFF;

	write_in_file(storage_file, 2 * 512, sector, 512); //(fat12->number_of_hidden_sectors + fat12->number_of_reserved_sectors) * 512


	clear_bytes(sector, 512);
	FAT_Data* init_file = sector;

/*
	copy_bytes(init_file->name, "ROOT       ", 11);
	init_file->attributes          = FAT_VOLUME_ID;
	//init_file->zero               = 8;
	//init_file->cluster_number_low = 3;
	init_file->file_size          = 0;
*/
/*
	++init_file;
	copy_bytes(init_file->name, "INIT    BIN", 11);
	init_file->attributes          = FAT_ARCHIVE;
	//init_file->zero               = 8;
	init_file->date_created       = 12345;
	init_file->date_last_write    = 12345;
	init_file->last_access_date   = 12345;
	init_file->cluster_number_low = 2;
	init_file->file_size          = 512;*/

	//write_in_file(storage_file, 3 * 512, sector, 512);//(fat12->number_of_hidden_sectors + fat12->number_of_reserved_sectors + fat12->number_of_FAT_tables * fat12->number_of_sectors_for_one_FAT) * 512
}


typedef struct
{
	Number32 signature1;              //0x41615252
	Byte     zeros[480];
	Number32 signature2;              //0x61417272
	Number32 number_of_free_clusters; //0xFFFFFFFF - неизвестно
	Number32 next_free_cluster;       //0xFFFFFFFF - нет информации о следующем свободном кластере
	Byte     zeros2[12];
	Number32 signature3;              //0xAA550000
}
FAT32_FSInfo;


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
	Number32 signature1;              //0x41615252
	Byte     zeros[480];
	Number32 signature2;              //0x61417272
	Number32 number_of_free_clusters; //0xFFFFFFFF - неизвестно
	Number32 next_free_cluster;       //0xFFFFFFFF - нет информации о следующем свободном кластере
	Byte     zeros2[12];
	Number32 signature3;              //0xAA550000
}
FAT32_FSInfo;


Byte bootcode_FAT32[] = {
	0x8C, 0xC8, 0x8E, 0xD8, 0x8E, 0xD0, 0x8E, 0xC0, 0x89, 0x46, 0xFE, 0xBC,
	0xFF, 0xFF, 0xBD, 0x00, 0x7C, 0x66, 0x31, 0xC0, 0x8B, 0x46, 0x0E, 0x66,
	0x8B, 0x5E, 0x1C, 0x66, 0x01, 0xC3, 0x66, 0x89, 0x5E, 0xF8, 0x66, 0x31,
	0xC0, 0x8A, 0x46, 0x10, 0x66, 0xF7, 0x66, 0x24, 0x66, 0x01, 0xD8, 0x66,
	0x89, 0x46, 0xF4, 0x66, 0x8B, 0x46, 0x2C, 0x66, 0x89, 0x46, 0xF0, 0x66,
	0xC7, 0x46, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x66, 0x8B, 0x46, 0xF8, 0x66,
	0xA3, 0x27, 0x7D, 0xE8, 0x68, 0x00, 0x66, 0xC7, 0x46, 0xE8, 0x00, 0x00,
	0x00, 0x00, 0xC7, 0x46, 0xFC, 0x00, 0x80, 0xE8, 0x77, 0x00, 0x83, 0xF8,
	0x00, 0x0F, 0x84, 0xEF, 0x00, 0xBB, 0x00, 0x80, 0x80, 0x3F, 0x00, 0x0F,
	0x84, 0xE5, 0x00, 0x89, 0xDE, 0xBF, 0xC3, 0x7D, 0xB9, 0x0B, 0x00, 0xF3,
	0xA6, 0x74, 0x0B, 0x83, 0xC3, 0x20, 0x81, 0xFB, 0x00, 0x82, 0x7D, 0xD7,
	0xEB, 0xE2, 0x8B, 0x47, 0x14, 0x66, 0xC1, 0xE0, 0x10, 0x8B, 0x47, 0x1A,
	0x66, 0x89, 0x46, 0xF0, 0xC7, 0x46, 0xEC, 0x00, 0x00, 0xC7, 0x46, 0xFC,
	0x00, 0x00, 0xC7, 0x46, 0xFE, 0x00, 0x20, 0xE8, 0x2F, 0x00, 0x83, 0xF8,
	0x00, 0x74, 0x06, 0x83, 0x46, 0xFE, 0x20, 0xEB, 0xF2, 0xEA, 0x00, 0x00,
	0x00, 0x20, 0xB4, 0x42, 0x8A, 0x56, 0x40, 0xBE, 0x1F, 0x7D, 0xCD, 0x13,
	0x0F, 0x82, 0x90, 0x00, 0xC3, 0x10, 0x00, 0x01, 0x00, 0x00, 0x7E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x46, 0xEC,
	0x3A, 0x46, 0x0D, 0x7C, 0x4B, 0xC7, 0x06, 0x14, 0x00, 0x00, 0x00, 0x66,
	0x8B, 0x46, 0xF0, 0x66, 0xC1, 0xE8, 0x07, 0x66, 0x39, 0x46, 0xE8, 0x74,
	0x1B, 0x66, 0x89, 0x46, 0xE8, 0x66, 0x03, 0x46, 0xF8, 0x66, 0xA3, 0x27,
	0x7D, 0xC7, 0x06, 0x23, 0x7D, 0x00, 0x7E, 0xC7, 0x06, 0x25, 0x7D, 0x00,
	0x00, 0xE8, 0xAA, 0xFF, 0x8B, 0x5E, 0xF0, 0x83, 0xE3, 0x7F, 0xC1, 0xE3,
	0x02, 0x81, 0xC3, 0x00, 0x7E, 0x66, 0x8B, 0x07, 0x66, 0x89, 0x46, 0xF0,
	0x66, 0x3D, 0xFF, 0xFF, 0xFF, 0x0F, 0x74, 0x29, 0x66, 0x8B, 0x46, 0xF0,
	0x66, 0x83, 0xE8, 0x02, 0xF6, 0x66, 0x0D, 0x66, 0x03, 0x46, 0xF4, 0x66,
	0xA3, 0x27, 0x7D, 0x8B, 0x46, 0xFC, 0xA3, 0x23, 0x7D, 0x8B, 0x46, 0xFE,
	0xA3, 0x25, 0x7D, 0xE8, 0x6C, 0xFF, 0xFF, 0x46, 0xEC, 0xB8, 0x01, 0x00,
	0xC3, 0x31, 0xC0, 0xC3, 0xB8, 0x00, 0xB8, 0x8E, 0xC0, 0xB0, 0x45, 0xB4,
	0x0C, 0xBB, 0x00, 0x00, 0x26, 0x89, 0x07, 0xB4, 0x00, 0xCD, 0x16, 0xCD,
	0x19, 0x4B, 0x45, 0x52, 0x4E, 0x45, 0x4C, 0x20, 0x20, 0x20, 0x20, 0x20
};



/*
Byte bootcode_FAT32[] = {
	0x33, 0xC9, 0x8E, 0xD1, 0xBC, 0xF4, 0x7B, 0x8E, 0xC1, 0x8E, 0xD9, 0xBD, 0x00, 0x7C, 0x88, 0x4E, 
	0x02, 0x8A, 0x56, 0x40, 0xB4, 0x08, 0xCD, 0x13, 0x73, 0x05, 0xB9, 0xFF, 0xFF, 0x8A, 0xF1, 0x66, 
	0x0F, 0xB6, 0xC6, 0x40, 0x66, 0x0F, 0xB6, 0xD1, 0x80, 0xE2, 0x3F, 0xF7, 0xE2, 0x86, 0xCD, 0xC0, 
	0xED, 0x06, 0x41, 0x66, 0x0F, 0xB7, 0xC9, 0x66, 0xF7, 0xE1, 0x66, 0x89, 0x46, 0xF8, 0x83, 0x7E, 
	0x16, 0x00, 0x75, 0x38, 0x83, 0x7E, 0x2A, 0x00, 0x77, 0x32, 0x66, 0x8B, 0x46, 0x1C, 0x66, 0x83, //6683C00C -> 6683C002
	0xC0, 0x02, 0xBB, 0x00, 0x80, 0xB9, 0x01, 0x00, 0xE8, 0x2B, 0x00, 0xE9, 0x48, 0x03, 0xA0, 0xFA, 
	0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x84, 0xC0, 0x74, 0x17, 0x3C, 0xFF, 0x74, 0x09, 0xB4, 0x0E, 
	0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEE, 0xA0, 0xFB, 0x7D, 0xEB, 0xE5, 0xA0, 0xF9, 0x7D, 0xEB, 
	0xE0, 0x98, 0xCD, 0x16, 0xCD, 0x19, 0x66, 0x60, 0x90, 0x90, 0x90, 0x90, 0x0F, 0x82, 0x4A, 0x00, //663B46F8 -> 90909090
	0x66, 0x6A, 0x00, 0x66, 0x50, 0x06, 0x53, 0x66, 0x68, 0x10, 0x00, 0x01, 0x00, 0x80, 0x7E, 0x02, 
	0x00, 0x0F, 0x85, 0x20, 0x00, 0xB4, 0x41, 0xBB, 0xAA, 0x55, 0x8A, 0x56, 0x40, 0xCD, 0x13, 0x0F, 
	0x82, 0x1C, 0x00, 0x81, 0xFB, 0x55, 0xAA, 0x0F, 0x85, 0x14, 0x00, 0xF6, 0xC1, 0x01, 0x0F, 0x84, 
	0x0D, 0x00, 0xFE, 0x46, 0x02, 0xB4, 0x42, 0x8A, 0x56, 0x40, 0x8B, 0xF4, 0xCD, 0x13, 0xB0, 0xF9, 
	0x66, 0x58, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58, 0xEB, 0x2A, 0x66, 0x33, 0xD2, 0x66, 0x0F, 0xB7, 
	0x4E, 0x18, 0x66, 0xF7, 0xF1, 0xFE, 0xC2, 0x8A, 0xCA, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 
	0xF7, 0x76, 0x1A, 0x86, 0xD6, 0x8A, 0x56, 0x40, 0x8A, 0xE8, 0xC0, 0xE4, 0x06, 0x0A, 0xCC, 0xB8, 
	0x01, 0x02, 0xCD, 0x13, 0x66, 0x61, 0x0F, 0x82, 0x54, 0xFF, 0x81, 0xC3, 0x00, 0x02, 0x66, 0x40, 
	0x49, 0x0F, 0x85, 0x71, 0xFF, 0xC3, 0x47, 0x42, 0x50, 0x4C, 0x46, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0D, 0x0A, 0x47, 0x42, 0x50, 0x4C, 0x46, 0x20, 0x69, 0x73, 0x20, 0x6D, 0x69, 0x73, 
	0x73, 0x69, 0x6E, 0x67, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73, 0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 
	0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 
	0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61, 0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 
	0xBF, 0xCC
};


Byte bootcode2_FAT32[] = {
	0x66, 0x0F, 0xB6, 0x46, 0x10, 0x66, 0x8B, 0x4E, 0x24, 0x66, 0xF7, 0xE1, 0x66, 0x03, 0x46, 0x1C, 
	0x66, 0x0F, 0xB7, 0x56, 0x0E, 0x66, 0x03, 0xC2, 0x66, 0x89, 0x46, 0xFC, 0x66, 0xC7, 0x46, 0xF4, 
	0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x8B, 0x46, 0x2C, 0x66, 0x83, 0xF8, 0x02, 0x0F, 0x82, 0xA6, 0xFC, 
	0x66, 0x3D, 0xF8, 0xFF, 0xFF, 0x0F, 0x0F, 0x83, 0x9C, 0xFC, 0x66, 0x50, 0x66, 0x83, 0xE8, 0x02, 
	0x66, 0x0F, 0xB6, 0x5E, 0x0D, 0x8B, 0xF3, 0x66, 0xF7, 0xE3, 0x66, 0x03, 0x46, 0xFC, 0xBB, 0x00, 
	0x82, 0x8B, 0xFB, 0xB9, 0x01, 0x00, 0xE8, 0x87, 0xFC, 0x38, 0x2D, 0x74, 0x1E, 0xB1, 0x0B, 0x56, 
	0xBE, 0x70, 0x7D, 0xF3, 0xA6, 0x5E, 0x74, 0x1B, 0x03, 0xF9, 0x83, 0xC7, 0x15, 0x3B, 0xFB, 0x72, 
	0xE8, 0x4E, 0x75, 0xDA, 0x66, 0x58, 0xE8, 0x65, 0x00, 0x72, 0xBF, 0x83, 0xC4, 0x04, 0xE9, 0x55, 
	0xFC, 0x00, 0x20, 0x83, 0xC4, 0x04, 0x8B, 0x75, 0x09, 0x8B, 0x7D, 0x0F, 0x8B, 0xC6, 0x66, 0xC1, 
	0xE0, 0x10, 0x8B, 0xC7, 0x66, 0x83, 0xF8, 0x02, 0x0F, 0x82, 0x3A, 0xFC, 0x66, 0x3D, 0xF8, 0xFF, 
	0xFF, 0x0F, 0x0F, 0x83, 0x30, 0xFC, 0x66, 0x50, 0x66, 0x83, 0xE8, 0x02, 0x66, 0x0F, 0xB6, 0x4E, 
	0x0D, 0x66, 0xF7, 0xE1, 0x66, 0x03, 0x46, 0xFC, 0xBB, 0x00, 0x00, 0x06, 0x8E, 0x06, 0x81, 0x80, 
	0xE8, 0x1D, 0xFC, 0x07, 0x66, 0x58, 0xC1, 0xEB, 0x04, 0x01, 0x1E, 0x81, 0x80, 0xE8, 0x0E, 0x00, 
	0x0F, 0x83, 0x02, 0x00, 0x72, 0xD0, 0x8A, 0x56, 0x40, 0xEA, 0x00, 0x00, 0x00, 0x20, 0x66, 0xC1, 
	0xE0, 0x02, 0xE8, 0x11, 0x00, 0x26, 0x66, 0x8B, 0x01, 0x66, 0x25, 0xFF, 0xFF, 0xFF, 0x0F, 0x66, 
	0x3D, 0xF8, 0xFF, 0xFF, 0x0F, 0xC3, 0xBF, 0x00, 0x7E, 0x66, 0x0F, 0xB7, 0x4E, 0x0B, 0x66, 0x33, 
	0xD2, 0x66, 0xF7, 0xF1, 0x66, 0x3B, 0x46, 0xF4, 0x74, 0x3A, 0x66, 0x89, 0x46, 0xF4, 0x66, 0x03, 
	0x46, 0x1C, 0x66, 0x0F, 0xB7, 0x4E, 0x0E, 0x66, 0x03, 0xC1, 0x66, 0x0F, 0xB7, 0x5E, 0x28, 0x83, 
	0xE3, 0x0F, 0x74, 0x16, 0x3A, 0x5E, 0x10, 0x0F, 0x83, 0xAB, 0xFB, 0x52, 0x66, 0x8B, 0xC8, 0x66, 
	0x8B, 0x46, 0x24, 0x66, 0xF7, 0xE3, 0x66, 0x03, 0xC1, 0x5A, 0x52, 0x8B, 0xDF, 0xB9, 0x01, 0x00, 
	0xE8, 0x9D, 0xFB, 0x5A, 0x8B, 0xDA, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};*/


void format_FAT32(File storage_file, Number64 storage_file_size)
{
	Byte   sector[512];
	FAT32* fat32;
	Number i;

	Number number_of_sectors = storage_file_size / 512 - 1;
	Number number_of_sectors_in_FAT_table = number_of_sectors / 128;


	clear_bytes(sector, 512);
	FAT32_FSInfo* fs_info = sector;
	fs_info->signature1              = 0x41615252;
	fs_info->signature2              = 0x61417272;
	fs_info->number_of_free_clusters = 0xFFFFFFFF;
	fs_info->next_free_cluster       = 0xFFFFFFFF;
	fs_info->signature3              = 0xAA550000;
	write_in_file(storage_file, 2 * 512, sector, 512);


	//clear_bytes(sector, 512);
	//copy_bytes(sector, bootcode2_FAT32, sizeof(bootcode2_FAT32));
	//write_in_file(storage_file, 3 * 512, sector, 512);


	Number32* fat = sector;

	clear_bytes(sector, 512);
	fat[0] = 0x0FFFFFF8;//fat12->type;
	fat[1] = 0x0FFFFFFF;
	fat[2] = 0x0FFFFFFF;

	write_in_file(storage_file, 3 * 512, sector, 512);


	clear_bytes(sector, 512);
	//FAT_Data* root_directory = sector;

	for(i = 0; i < number_of_sectors_in_FAT_table - 1; ++i)
	//for(i = 0; i < 64; ++i)
		write_in_file(storage_file, (4 + i) * 512, sector, 512);

	write_in_file(storage_file, (3 + number_of_sectors_in_FAT_table) * 512, sector, 512);


	clear_bytes(sector, 512);
	
	sector[0] = 0xEB;
	sector[1] = 0x58; //jmp 0x58
	sector[2] = 0x90; //nop

	fat32 = sector + 3;

	copy_bytes(fat32->os_name, "MSDOS5.0", 8);
	fat32->bytes_in_sector            = 512;
	fat32->sectors_in_cluster         = 1;
	fat32->number_of_reserved_sectors = 2;
	fat32->number_of_FAT_tables       = 1;
	fat32->type                       = 0xF8;
	fat32->sectors_per_track          = 63;
	fat32->number_of_heads            = 255;
	fat32->number_of_hidden_sectors   = 1;
	fat32->number_of_sectors          = number_of_sectors;

	fat32->number_of_sectors_in_FAT_table  = number_of_sectors_in_FAT_table;
	//fat32->flags                           = 3;
	fat32->root_directory_cluster          = 2;
	fat32->FSInfo_sector_number            = 1;
	fat32->backup_sector_number            = 0;
	fat32->drive_number                    = 0x80;
	fat32->signature                       = 0x29;
	fat32->volume_serial_number            = 2600000000;
	copy_bytes(fat32->volume_label, "NO NAME    ", 11);
	copy_bytes(fat32->file_system_name, "FAT32   ", 8);

	copy_bytes(sector + 90, bootcode_FAT32, sizeof(bootcode_FAT32));

	sector[510] = 0x55;
	sector[511] = 0xAA;

	write_in_file(storage_file, 512, sector, 512);
}


typedef struct
{
	File     storage_file;
	Number64 storage_file_size;

	Number32 FAT_start_sector;
	Number32 data_start_sector;
	Number32 sectors_in_cluster;
	Number32 number_of_clusters;

	FAT_Data current_directory;

	Number32 current_FAT_sector;
	Number32 FAT[128];
}
FAT_File_System;


void read_FAT_sector(FAT_File_System* file_system, Number32 cluster_number, Byte* sector, Number32 offset)
{
	read_from_file(file_system->storage_file, (file_system->data_start_sector + (cluster_number - 2) * file_system->sectors_in_cluster + offset) * 512, sector, 512);
}


void write_FAT_sector(FAT_File_System* file_system, Number32 cluster_number, Byte* sector, Number32 offset)
{
	write_in_file(file_system->storage_file, (file_system->data_start_sector + (cluster_number - 2) * file_system->sectors_in_cluster + offset) * 512, sector, 512);
}


void open_FAT_File_System(FAT_File_System* file_system, File storage_file)
{
	Byte     sector[512];
	FAT32*   fat32;

	file_system->storage_file = storage_file;

	read_from_file(storage_file, 512, sector, 512);
	fat32 = sector + 3;

	file_system->FAT_start_sector          = fat32->number_of_hidden_sectors + fat32->number_of_reserved_sectors;
	file_system->data_start_sector         = file_system->FAT_start_sector + fat32->number_of_FAT_tables * fat32->number_of_sectors_in_FAT_table;
	file_system->sectors_in_cluster        = fat32->sectors_in_cluster;
	file_system->number_of_clusters        = fat32->number_of_sectors / fat32->sectors_in_cluster;

	clear_bytes(&file_system->current_directory, sizeof(file_system->current_directory));
	file_system->current_directory.cluster_number_low = fat32->root_directory_cluster;
	file_system->current_directory.cluster_number_high = fat32->root_directory_cluster >> 16;

	file_system->current_FAT_sector        = 0;
	read_from_file(file_system->storage_file, (file_system->FAT_start_sector + file_system->current_FAT_sector) * 512, file_system->FAT, 512);
}


Number32 next_FAT_cluster(FAT_File_System* file_system, Number32 current_cluster)
{
	if(current_cluster / 128 != file_system->current_FAT_sector)
	{
		file_system->current_FAT_sector = current_cluster / 128;
		read_from_file(file_system->storage_file, (file_system->FAT_start_sector + file_system->current_FAT_sector) * 512, file_system->FAT, 512);
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
		read_from_file(file_system->storage_file, file_system->current_FAT_sector * 512, file_system->FAT, 512);

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
	write_in_file(file_system->storage_file, file_system->current_FAT_sector * 512, file_system->FAT, 512);
	
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
		read_from_file(file_system->storage_file, (file_system->FAT_start_sector + file_system->current_FAT_sector) * 512, file_system->FAT, 512);
	}

	file_system->FAT[current_cluster % 128] = allocated_cluster;
	write_in_file(file_system->storage_file, (file_system->FAT_start_sector + file_system->current_FAT_sector) * 512, file_system->FAT, 512);

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
	print(_null_terminated_words, name, "\n");
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
		print(_null_terminated_words, name ," exists\n");
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


#endif//FAT_STORAGE

/*
#include <storage/MBR.c>

Number main()
{
	File     storage_file;
	Number64 storage_file_size;

	storage_file = open_file(L"storage");//open_file(L"\\\\.\\PhysicalDrive1");
	if(!storage_file)
	{
		log_error("can't open \"storage\" file");
		return 1;
	}
	storage_file_size = get_file_size(storage_file);
	print("storage size: ", _Number64_triplets, storage_file_size, " bytes\n")

	//create_partition(storage_file, storage_file_size);
	//format_FAT12(storage_file, storage_file_size);
	//format_FAT32(storage_file, storage_file_size);
	//add_files(storage_file, storage_file_size);
	print_files_tree(storage_file, storage_file_size);

	close_file(storage_file);

	return 0;
}*/