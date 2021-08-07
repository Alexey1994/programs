org 0x20000

mov ax, cs
mov ds, ax
mov ss, ax

xor ax, ax
mov es, ax

;копирование кода настройки сегментных регистров 32 битного режима
mov si, code_32_setup
mov di, 0x8000
mov cx, end_code_32_setup - code_32_setup
rep movsb

;информация о VESA
;mov ax, 0x4F00
;mov di, 0x7E00
;int 10h
;cmp ax, 0x004F je video_error
;поиск режима
;mov eax, 0x7E00 + 14

;установка видеорежима 1024x768x256
;mov ax, 0x4F02
;mov bx, 0x4105 ;1024x768x256
;int 10h

;получение информации о видеорежиме
;mov ax, 0x4F01
;mov cx, 0x4105 ;1024x768x256
;mov di, 0x7D00
;int 10h

cli

;разрешить A20
in al, 0x92
or al, 2
out 0x92, al

lgdt [GDT_pointer]

;перейти в 32 битный режим
mov eax, cr0
or eax, 1
mov cr0, eax
jmp 8:0x8000


use32
code_32_setup:
	mov eax, 16
	mov ds, eax
	mov ss, eax
	mov es, eax
	mov esp, 0x60000
	jmp 8:code_32
end_code_32_setup:

code_32:
	call kernel_main
no_actions:
	hlt
	jmp no_actions


align 16
GDT:
	; dummy
	dq 0

	; CODE (CS register = 8)
	dw 0xffff     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10011010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    если сегмент в памяти то 1
	              ; 1    сегмент исполняемый
	              ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода(1 - разрешено, 0 - запрещено)
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    AVL(?)
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес

	; DATA (DS register = 16)
	dw 0xffff     ; размер сегмента
	dw 0          ; базовый адрес
	db 0          ; базовый адрес
	db 0b10010010 ; 1    сегмент правильный(должно быть 1)
	              ; 00   уровень привилегий(меньше - больше привилегий)
	              ; 1    если сегмент в памяти то 1
	              ; 0    сегмент исполняемый
	              ; 0    направление для сегмента данных либо возможность перехода с низких привилегий на высокие для сегмента кода
	              ; 1    разрешение на чтение для сегмента кода, разрешение на запись для сегмента данных
	              ; 0    бит доступа к сегменту, устанавливается процессором(рекомендуется 0)
	db 0b11001111 ; 1    гранулярность(если 0, то размер адреса равен размеру сегмента кода, если 1 то размеру сегмента кода * 4096)
	              ; 1    размер, если 0 и 64 битный режим(следующий бит) = 0, то селектор определяет 16 битный режим, если 1 - 32 битный. Если 64 битный режим равен 1, то должен быть равен 0(значение 1 зарезервировано, будет генерировать исключение)
	              ; 0    64 битный режим
	              ; 0    AVL(?)
	              ; 1111 размер сегмента
	db 0          ;      базовый адрес

GDT_pointer:
	dw $ - GDT ;размер
	dd GDT     ;адрес



align 32
kernel_main: