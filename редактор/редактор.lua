current_path = 'D:'

drives_view = {}
files_view = {}


default_font = font(15, 'Droid Sans')


main_class = window_class({
	name       = 'main',
	background = rgb(255, 255, 255)
})

y = 0

main_window = window({
	class  =  main_class,
	name   = 'Редактор',
	x      = 0,
	y      = 0,
	width  = 430,
	height = 430,

	on_paint = function(paint)
		--print('repaint')
	end,

	on_left_mouse_button_up = function()
		y = y - 100
		main_window:scroll(0, y, 0, 100, 100, 100)
	end,

	on_destroy = function()
		exit(0)
	end
})


drive_class = window_class({
	name       = 'drive',
	background = rgb(196, 196, 196)
})

active_drive_class = window_class({
	name       = 'active-drive',
	background = rgb(232, 232, 232)
})

function draw_drive(index, name)
	return window({
		parent = main_window,
		class  = drive_class,
		name   = 'Диск',
		x      = 10 + (index - 1) * 50,
		y      = 10,
		width  = 40,
		height = 30,

		on_paint = function(paint)
			paint:text(%name, default_font, 8, 4, 40, 25)
		end,

		on_left_mouse_button_up = function()
			drives_view[%index].class = active_drive_class
			current_path = %name
			draw_files(current_path .. '\\*')
		end
	})
end


file_class = window_class({
	name       = 'file',
	background = rgb(232, 232, 232)
})

function draw_file(index, file)
	local w = window({
		parent = main_window,
		class  = file_class,
		name   = 'Файл',
		x      = 10,
		y      = 10 + 32 + (index - 1) * 33,
		width  = 400,
		height = 32,

		on_paint = function(paint)
			paint:text(%file.name, default_font, 8, 4, 400, 25)
		end,

		on_left_mouse_button_up = function()
			if %file.is_folder then
				current_path = current_path .. '\\' .. %file.name
				draw_files(current_path .. '\\*')
			else
				execute('cmd /C ""' .. current_path .. '\\' .. %file.name .. '""')
			end
		end
	})

	return w
end


drives = get_drives()

for index, drive in drives do
	drives_view[index] = draw_drive(index, drive)
end


function draw_files(path)
	files = get_files(path)

	for index, file_view in files_view do
		file_view:destroy()
	end

	files_view = {}

	local view_index = 1
	for index, file in files do
		if file.name ~= '.' and file.name ~= '..' then
			files_view[view_index] = draw_file(view_index, file)
			view_index = view_index + 1
		end
	end
end

draw_files(current_path .. '\\*')


main_window:show()
--main_window:repaint()

print(1)