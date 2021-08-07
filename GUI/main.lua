default_font = font(15, 'Droid Sans')


main_class = window_class({
	name       = 'main',
	background = image('a.bmp')--rgb(255, 255, 255)
})

a = window({
	class  =  main_class,
	name   = 'Красная',
	x      = 0,
	y      = 0,
	width  = 512,
	height = 512,

	on_destroy = function()
		exit(0)
	end
})

a:show()


button_class = window_class({
	name       = 'button',
	background = rgb(196, 196, 196)
})

b = window({
	parent =  a,
	class  =  button_class,
	name   = 'Шапочка',
	x      = 10,
	y      = 10,
	width  = 128,
	height = 25,

	on_paint = function(paint)
		--paint:rectangle(0, 5, 10, 10, rgb(255, 0, 0))
		--paint:rectangle(20, 15, 10, 10, rgb(255, 0, 255))
		paint:text('Жмякни тут', default_font, 10, 3, 100, 25)
	end,

	on_left_mouse_button_up = function()
		print('click')
	end
})

b:show()