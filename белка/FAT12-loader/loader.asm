;FAT_start_sector = number_of_hidden_sectors + number_of_reserved_sectors
;directories_start_sector = FAT_start_sector + number_of_FATs * size_of_FAT
;number_of_sectors_in_root = number_of_directories / 16
;data_start_sector = directories_start_sector + number_of_sectors_in_root

;read_next_directory_sector: !number_of_sectors_in_root => read_error
;                            read_next_sector()
;                            --number_of_sectors_in_root
;                            SI = 0x7E00
;test_next_directory:        compare_string(SI, "NTLDR      ") => init_file_found
;                            SI += 32
;                            SI > 0x7E00 + 512 => read_next_directory_sector
;                            test_next_directory

%define FAT_directories 0x7E00
%define FAT             0x8000

;локальные переменные
%define FAT_start_sector          4
%define directories_start_sector  8
%define data_start_sector         12
%define number_of_sectors_in_root 16
%define current_file_cluster      18

;переменные заголовка FAT
%define bytes_in_sector               11
%define sectors_in_cluster            13
%define number_of_reserved_sectors    14
%define number_of_FAT_tables          16
%define number_of_directories         17
%define number_of_sectors_16          19
%define type                          21
%define number_of_sectors_for_one_FAT 22
%define sectors_per_track             24
%define number_of_heads               26
%define number_of_hidden_sectors      28
%define number_of_sectors_32          32
%define drive_number                  36

;переменные файлового заголовка FAT
%define first_file_cluster_low 26
%define file_size              28


org 0x7C3E

                             mov ax, cs
                             mov ds, ax
                             mov ss, ax
                             mov es, ax
                             mov sp, 0xFFFF
                             mov bp, 0x7C00

                             xor eax, eax
                             mov ax, [bp + number_of_reserved_sectors]
                             mov ebx, [bp + number_of_hidden_sectors]
                             add ebx, eax
                             mov [bp - FAT_start_sector], ebx ;FAT_start_sector = number_of_hidden_sectors + number_of_reserved_sectors

                             xor eax, eax
                             mov ax, [bp + number_of_sectors_for_one_FAT]
                             mul byte [bp + number_of_FAT_tables]
                             add eax, ebx
                             mov [bp - directories_start_sector], eax ;directories_start_sector = FAT_start_sector + number_of_FATs * size_of_FAT

                             xor eax, eax
                             mov ax, [bp + number_of_directories]
                             shr ax, 4
                             mov [bp - number_of_sectors_in_root], ax ;number_of_sectors_in_root = number_of_directories / 16
                             add eax, [bp - directories_start_sector]
                             mov [bp - data_start_sector], eax ;data_start_sector = directories_start_sector + number_of_directories / 16

                             mov eax, [bp - directories_start_sector]
                             mov [start_sector], eax
read_next_directory_sector:  cmp word [bp - number_of_sectors_in_root], 0
                             je read_error
                             call read_next_sector
                             dec word [bp - number_of_sectors_in_root]
                             mov bx, FAT_directories
test_next_directory:         mov si, bx
                             mov di, init_name
                             mov cx, 11
                             repe cmpsb
                             je init_file_found
                             add bx, 32
                             cmp bx, FAT_directories + 512
                             je read_next_directory_sector
                             jmp test_next_directory


init_file_found:             xor eax, eax
                             mov ax, [bx + first_file_cluster_low]
                             mov [bp - current_file_cluster], ax

                             mov eax, [bp - FAT_start_sector]
                             mov [start_sector], eax
                             mov [buffer_address_offset], word FAT
                             call read_next_sector
                             mov [buffer_address_offset], word FAT + 512
                             call read_next_sector

                             mov [buffer_address_offset], word 0
                             mov [buffer_address_segment], word 0x2000

                             mov ax, [bp - current_file_cluster]

read_next_file_sector:       cmp ax, 0xFFF
                             je init_file_loaded

                             sub eax, 2
                             mul byte [bp + sectors_in_cluster]
                             add eax, [bp - data_start_sector]
                             mov [start_sector], eax
                             call read_sector
                             add [buffer_address_offset], word 512
                             cmp [buffer_address_offset], word 0
                             jne get_next_cluster
                             add [buffer_address_segment], word 0x1000

get_next_cluster:            mov ax, [bp - current_file_cluster]
                             mov bx, ax
                             mov cx, ax
                             shr ax, 1
                             add bx, ax
                             mov ax, [bx + FAT]
                             and cx, 1
                             cmp cx, 1
                             je transform_odd_cluster
                             and ax, 0xFFF
                             jmp update_current_file_cluster
transform_odd_cluster:       shr ax, 4
update_current_file_cluster: mov [bp - current_file_cluster], ax
                             jmp read_next_file_sector

init_file_loaded:      
        mov ax, 0xB800
		mov es, ax
		mov al, 1
		mov ah, 15
		mov bx, 0
		mov [es:bx], ax

		xor edx, edx
		mov dl, [bp + drive_number]

		mov ax, 0
		mov es, ax
		mov edi, FAT_directories

		mov ecx, 512

		jmp 0x2000:0


read_sector:
	mov ah, 42h
	mov dl, [bp + drive_number]
	mov si, LBA_packet
	int 13h
	jc read_error
	ret

	read_error:
		mov ax, 0xB800
		mov es, ax
		mov al, 1
		mov ah, 12
		mov bx, 0
		mov [es:bx], ax
		mov ah, 0
		int 16h
		int 19h

read_next_sector:
	call read_sector
	inc dword [start_sector]
	ret

	LBA_packet:
		size                   db 16
		zero                   db 0
		number_of_sectors      dw 1
		buffer_address_offset  dw FAT_directories
		buffer_address_segment dw 0
		start_sector           dq 0

init_name:
	db "NTLDR      "