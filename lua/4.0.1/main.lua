default_font = font(15, 'Droid Sans')


main_class = window_class({
	name       = 'main',
	background = rgb(255, 255, 255)
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