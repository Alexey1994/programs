;FAT_start_sector = number_of_hidden_sectors + number_of_reserved_sectors
;data_start_sector = FAT_start_sector + number_of_FAT_tables * number_of_sectors_in_FAT_table

;read_next_directory_sector: read_next_sector()
;                            BX = 0x7E00
;test_next_directory:        !BX[0] => read_error
;                            compare_string(BX, "NTLDR      ") => init_file_found
;                            BX += 32
;                            BX > 0x7E00 + 512 => read_next_directory_sector
;                            test_next_directory

;read_next_sector:    current_cluster_offset < sectors_in_cluster => read_cluster_sector
;                     current_cluster = FAT[current_cluster]
;                     current_cluster_offset = 0
;read_cluster_sector: read_sector()
;                     current_cluster_offset = current_cluster_offset + 1




%define FAT_data          0x8000
%define FAT               0x7E00
%define init_file_segment 0x2000
%define init_file_offset  0

;локальные переменные
%define data_segment                  2
%define data_offset                   4
%define FAT_start_sector              8
%define data_start_sector             12
%define current_cluster               16
%define current_cluster_offset        20
%define current_FAT_sector            24

;переменные заголовка FAT
%define sectors_in_cluster             13
%define number_of_reserved_sectors     14
%define number_of_FAT_tables           16
%define number_of_hidden_sectors       28
%define number_of_sectors_in_FAT_table 36
%define root_directory_cluster         44
%define drive_number                   64

;переменные файлового заголовка FAT
%define FAT_data_cluster_high 20
%define FAT_data_cluster_low  26
%define FAT_data_size         28


org 0x7C5A

mov ax, cs
mov ds, ax
mov ss, ax
mov es, ax
mov [bp - data_segment], ax
mov sp, 0xFFFF
mov bp, 0x7C00

xor eax, eax
mov ax, [bp + number_of_reserved_sectors]
mov ebx, [bp + number_of_hidden_sectors]
add ebx, eax
mov [bp - FAT_start_sector], ebx ;FAT_start_sector = number_of_hidden_sectors + number_of_reserved_sectors

xor eax, eax
mov al, [bp + number_of_FAT_tables]
mul dword [bp + number_of_sectors_in_FAT_table]
add eax, ebx
mov [bp - data_start_sector], eax ;data_start_sector = FAT_start_sector + number_of_FAT_tables * number_of_sectors_in_FAT_table

mov eax, [bp + root_directory_cluster]
mov [bp - current_cluster], eax
mov dword [bp - current_cluster_offset], 0

;чтение первой FAT таблицы
mov eax, [bp - FAT_start_sector]
mov [start_sector], eax
;mov word [buffer_address_offset], FAT
;mov word [buffer_address_segment], 0
call read_sector
mov dword [bp - current_FAT_sector], 0

mov word [bp - data_offset], FAT_data

read_next_directory_sector:
	call read_next_FAT_sector
	cmp ax, 0
	je read_error
	mov bx, FAT_data
test_next_directory:
	cmp byte [bx], 0
	je read_error

	mov si, bx
	mov di, init_name
	mov cx, 11
	repe cmpsb
	je init_file_found

	add bx, 32
	cmp bx, FAT_data + 512
	jge read_next_directory_sector
	jmp test_next_directory


init_file_found:
	mov ax, [bx + FAT_data_cluster_high]
	shl eax, 16
	mov ax, [bx + FAT_data_cluster_low]
	mov [bp - current_cluster], eax
	mov word [bp - current_cluster_offset], 0

	mov word [bp - data_offset], init_file_offset
	mov word [bp - data_segment], init_file_segment

load_next_chunk:
	call read_next_FAT_sector
	cmp ax, 0
	je init_file_loaded
	add word [bp - data_segment], 32
	jmp load_next_chunk

init_file_loaded:
	jmp init_file_segment:init_file_offset
	;mov ax, 0xB800
	;mov es, ax
	;mov al, 1
	;mov ah, 15
	;mov bx, 0
	;mov [es:bx], ax
	;mov ah, 0
	;int 16h
	;int 19h


read_sector:
	mov ah, 42h
	mov dl, [bp + drive_number]
	mov si, LBA_packet
	int 13h
	jc read_error
	ret

	LBA_packet:
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 1
		buffer_address_offset  dw FAT
		buffer_address_segment dw 0
		start_sector           dq 0


	;если в ax 0, то конец кластера
read_next_FAT_sector:
		mov ax, [bp - current_cluster_offset]
		cmp al, [bp + sectors_in_cluster]
		jl read_cluster_sector
		mov word [current_cluster_offset], 0

;if current_cluster / 128 != current_FAT_sector
;   current_FAT_sector = current_cluster / 128;
;   read_sector(FAT_start_sector + current_FAT_sector, FAT)

		;читает следующий кластер
		mov eax, [bp - current_cluster]
		shr eax, 7
		cmp [bp - current_FAT_sector], eax
		je get_next_cluster_from_FAT
		mov [bp - current_FAT_sector], eax ;current_FAT_sector = cluster_number / 128

		add eax, [bp - FAT_start_sector]
		mov [start_sector], eax
		mov word [buffer_address_offset], FAT
		mov word [buffer_address_segment], 0
		call read_sector

;current_cluster = FAT[current_cluster % 128];
	get_next_cluster_from_FAT:
		mov bx, [bp - current_cluster]
		and bx, 0b1111111
		shl bx, 2
		add bx, FAT
		mov eax, [bx]
		mov [bp - current_cluster], eax ;current_cluster = FAT[current_cluster % 128]
		cmp eax, 0x0FFFFFFF
		je end_of_cluster

	read_cluster_sector:
		mov eax, [bp - current_cluster]
		sub eax, 2
		mul byte [bp + sectors_in_cluster]
		add eax, [bp - data_start_sector]
		;add eax, [bp - current_cluster_offset]
		mov [start_sector], eax ;start_sector = data_start_sector + (cluster_number - 2) * sectors_in_cluster + offset

		mov ax, [bp - data_offset]
		mov [buffer_address_offset], ax
		mov ax, [bp - data_segment]
		mov [buffer_address_segment], ax
		call read_sector

		inc word [bp - current_cluster_offset]

		mov ax, 1
		ret

	end_of_cluster:
		xor ax, ax
		ret


read_error:
	mov ax, 0xB800
	mov es, ax
	mov al, 'E'
	mov ah, 12
	mov bx, 0
	mov [es:bx], ax
	mov ah, 0
	int 16h
	int 19h

init_name:
	db "GBPLF      "
	;db "LOADER     "