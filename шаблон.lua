function bind(initial_value)
	local context = {
		value = initial_value,
		binds = {}
	}

	return {
		set = function(new_value)
			for index, bind in %context.binds do
				bind(new_value)
			end

			%context.value = new_value
		end,

		get = function()
			return %context.value
		end,

		change = function(on_change)
			%context.binds[length(%context.binds) + 1] = on_change
		end
	}
end


function get_value(options, field_name)
	if is_table(options[field_name]) then
		return options[field_name].get()
	else
		return options[field_name]
	end
end

function update_value(options, context, name, action)
	if is_table(options[name]) then
		options[name].change(function(new_value)
			%context[%name] = new_value
			%action(%context)
		end)
	end
end


body_class = window_class({
	name       = 'body_class',
	background = rgb(255, 255, 255)
})

default_font = font(15, 'Droid Sans')

function element(options, inner)
	local context = {
		window = nil,

		name   = get_value(options, 'name'),
		x      = get_value(options, 'x'),
		y      = get_value(options, 'y'),
		width  = get_value(options, 'width'),
		height = get_value(options, 'height'),
		text   = get_value(options, 'text'),

		on_click = get_value(options, 'on_click'),
	}

	update_value(options, context, 'name', function(context)
		context.window:set_text(context.name)
	end)

	update_value(options, context, 'x', function(context)
		context.window:move(context.x, context.y, context.width, context.height)
	end)

	update_value(options, context, 'y', function(context)
		context.window:move(context.x, context.y, context.width, context.height)
	end)

	update_value(options, context, 'width', function(context)
		context.window:move(context.x, context.y, context.width, context.height)
	end)

	update_value(options, context, 'height', function(context)
		context.window:move(context.x, context.y, context.width, context.height)
	end)

	return {
		show = function(parent)
			local c = %context

			if not %context.window then
				%context.window = window({
					parent = parent,
					class  = body_class,
					name   = %context.name or '',
					x      = %context.x,
					y      = %context.y,
					width  = %context.width,
					height = %context.height,

					on_paint = function(paint)
						if %c.text then
							paint:text(%c.text, default_font, 0, 0, 400, 25)
						end
					end,

					on_left_mouse_button_up = function()
						if %c.on_click then
							%c.on_click()
						end
					end,

					on_destroy = function()
						exit(0)
					end
				})
			end

			for index, element in %inner do
				element.show(%context.window)
			end

			%context.window:show()
		end
	}
end




function flex(options, inner)
	return {
		show = function(parent_window)
		end
	}
end


function a(options, inner)
	return element({x = 0, y = 100, width = 100, height = 20}, {
		element({x = 0, y = 0, width = 100, height = 20}, {

		}),
		element({x = 100, y = 100, width = 100, height = 20}, {

		})
	})
end


t_height = bind(512)
t_name = bind('Окошко')

t = element({
		name = t_name,

		x      = 0,
		y      = 0,
		width  = 512,
		height = t_height,

		on_click = function()
			%t_name.set('Окошко нажалось нежно')
			%t_height.set(200)
		end
	}, {
		element({
			x      = 0,
			y      = 0,
			width  = 100,
			height = 20,
			text = 'Вот'
		}, {
			element({
				x      = 50,
				y      = 0,
				width  = 100,
				height = 20,
				text = 'так'
			}, {

			})
		})
	}
)

t:show()


e = element({
	x      = 0,
	y      = 0,
	width  = 100,
	height = 100
}, {
	flex({}, {
		element({text = 'abc'}, {})
	})
})

--e:show()