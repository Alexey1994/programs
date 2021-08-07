/*
;FAT12 bootmgr
org 0x7C3E

		;инициализация
        xor cx, cx
        mov ss, cx
        mov sp, 0x7bf0
        mov ds, cx
        mov ax, 0x2000
        mov es, ax
        cld
        mov bp, 0x7c00

		;видимо проверки для неактивного раздела
        cmp [bp+0x24], cl ;drive number
        jnl 0x3B
        mov ax, cx
        cwd
        call 0x159
        jc 0x3B
        sub bx, 58
        mov eax, [0x7c1c] ;hidden sectors
0x26:   cmp eax, [es:bx]
        mov dl, [es:bx-4]
        jnz 0x36
        or  dl, 2
        mov [bp+0x2], dl
0x36:   add bl, 16
        jnc 0x26

0x3B:   xor cx, cx
        mov al, [bp+0x10] ;number of FATs
        cbw
        mul [bp+0x16] ;FAT size in sectors
        add ax, [bp+0x1c] ;hidden sectors low
        adc dx, [bp+0x1e] ;hidden sectors high
        add ax, [bp+0xe] ;reserved sectors
        adc dx, cx
        mov si, [bp+0x11] ;number of directories in root
        pusha
        	mov [bp-0x4], ax ;root start sector low
        	mov [bp-0x2], dx ;root start sector high
        	mov ax, 32
        	mul si
        	mov bx, [bp+0xb] ;bytes per sector
        	add ax, bx
        	dec ax
        	div bx
        	add [bp-4], ax
        	adc [bp-2], cx
        popa
0x6D:   mov di, 0
        call 0x159
        jc 0xAE
0x75:   cmp [es:di], ch
        jz 0x91
        pusha
        	mov cl, 11
        	mov si, 0x7da1 ;NTLDR
        	repe cmpsb
        popa
        jz 0xB7
        dec si
        jz 0x91
        add di, 32
        cmp di, bx
        jc 0x75
        jmp 0x6D

;печать ошибок на экран
0x91:   mov al, [0x7dfb] ;NTLDR is missing.
0x94:   mov ah, 0x7d
        mov si, ax
0x98:   lodsb
        cbw
        inc ax
        jz 0xA9
        dec ax
        jz 0xB3
        mov ah, 0xe
        mov bx, 0x7
        int 0x10
        jmp 0x98
0xA9:   mov al, [0x7dfd] ;Press any key to restart.
        jmp 0x94
0xAE:   mov al, [0x7dfc] ;Disk error
        jmp 0x94
0xB3:   int 0x16
        int 0x19

0xB7:   mov dx, [es:di+0x1a]
        push dx
        mov al, 1
        mov bx, 0
        call 0xFF
        jc 0xAE
        pop bx
        mov dl, [bp+0x24] ;drive number
        mov si, 0x7c0b
        mov di, sp
        mov [bp-16], 0x7d3d
        mov [bp-12], 0x7d29
        mov cx, ds
        mov [bp-14], cx
        mov [bp-10], cx
        mov byte [0x7d96], 0xcb
        jmp 0x2000:0x3

        movzx cx, al
        mov eax, [bp-8]
        add eax, [bp+0x1c] ;hidden sectors
        mov edx, eax
        shr edx, 16
        jmp 0x15D

0xFF:
        movzx cx,al
        dec dx
        dec dx
        mov al, [bp+0xd] ;sectors per cluster
        xor ah, ah
        mul dx
        add ax, [bp-4]
        adc dx, [bp-2]
        jmp 0x15D

0x113:  push dx      ;стартовый сектор
        push ax      ;сегмент
        push es      ;смещение
        push bx      ;количество секторов
        push byte 1  ;страннно, нужен 0
        push byte 16 ;размер структуры LBA
        xchg ax, cx
        mov ax, [bp+0x18] ;sectors per track
        xchg ax, si
        xchg ax, dx
        xor dx, dx
        div si
        xchg ax, cx
        div si
        inc dx
        xchg cx, dx
        div word [bp+0x1a] ;number of heads
        mov dh, dl
        mov ch, al
        ror ah, 2
        or cl, ah
        mov ax, 0x201
        cmp byte [bp+0x2], 0xe
        jnz 0x144
        mov ah, 0x42
        mov si, sp
0x144:  mov dl, [bp+0x24] ;drive number
        int 0x13
        popa
        popa
        jc 0x158
        inc ax
        jnz 0x151
        inc dx
0x151:  add bx, [bp+0xb] ;bytes per sector
        dec cx
        jnz 0x15D
        clc
0x158:  ret

0x159:
        inc cx
        mov bx, 0
0x15D:  pusha
        o32 push byte 0
        jmp short 0x113

0x163:  "NTLDR      " ;0x7D63 в памяти
*/


#include <storage/MBR.c>
#include <storage/FAT.c>
#include <log.c>


void print_files_tree(File storage_file, Number64 storage_file_size)
{
	FAT_File_System file_system;
	FAT_Data        file;

	open_FAT_File_System(&file_system, storage_file, storage_file_size);
	
	//create_FAT_directory(&file_system, L"A");
	//open_FAT_directory(&file_system, L"A");
/*
	create_FAT_directory(&file_system, L"A");
	create_FAT_directory(&file_system, L"B");
	create_FAT_directory(&file_system, L"C");
	create_FAT_directory(&file_system, L"D");
	create_FAT_directory(&file_system, L"E");
	create_FAT_directory(&file_system, L"F");
	create_FAT_directory(&file_system, L"G");
	create_FAT_directory(&file_system, L"H");
	create_FAT_directory(&file_system, L"I");
	create_FAT_directory(&file_system, L"J");
	create_FAT_directory(&file_system, L"K");
	create_FAT_directory(&file_system, L"L");
	create_FAT_directory(&file_system, L"M");
	create_FAT_directory(&file_system, L"N");
	create_FAT_directory(&file_system, L"O");
	create_FAT_directory(&file_system, L"P");
	create_FAT_directory(&file_system, L"Q");
	create_FAT_directory(&file_system, L"R");
	create_FAT_directory(&file_system, L"S");
	create_FAT_directory(&file_system, L"TA");
	create_FAT_directory(&file_system, L"TB");
	create_FAT_directory(&file_system, L"TC");
	create_FAT_directory(&file_system, L"TD");
	create_FAT_directory(&file_system, L"TE");
	create_FAT_directory(&file_system, L"TF");
	create_FAT_directory(&file_system, L"TG");
	create_FAT_directory(&file_system, L"TH");
	create_FAT_directory(&file_system, L"TI");
	create_FAT_directory(&file_system, L"TJ");
	create_FAT_directory(&file_system, L"TK");
	create_FAT_directory(&file_system, L"TL");
	create_FAT_directory(&file_system, L"TM");
	create_FAT_directory(&file_system, L"TN");
	create_FAT_directory(&file_system, L"TO");
	create_FAT_directory(&file_system, L"TP");
	create_FAT_directory(&file_system, L"TQ");
	create_FAT_directory(&file_system, L"TR");
	create_FAT_directory(&file_system, L"TS");*/
	//create_FAT_file(&file_system, L"INIT", 64);
	list_FAT_directory(&file_system);
/*
	if(open_FAT_file(&file_system, L"INIT", &file))
	{
		Byte sector[512];
		read_FAT_sector(&file_system, file.cluster_number_low + (file.cluster_number_high << 16), sector, 0);
	
		print(sector);
	}*/
}


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

	create_partition(storage_file, storage_file_size);
	//format_FAT12(storage_file, storage_file_size);
	format_FAT32(storage_file, storage_file_size);
	//add_files(storage_file, storage_file_size);
	print_files_tree(storage_file, storage_file_size);

	close_file(storage_file);

	return 0;
}