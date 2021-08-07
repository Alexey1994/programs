for i = 1, length(events) do
	local event = events[i]
	local type = event.type
	
	if type == MOUSE_MOVE_MESSAGE then
		--print(events[i])
	elseif type == KEY_DOWN_MESSAGE then
		local key_code = event.parameter1

		if key_code == 65 then
			move_left = true
		elseif key_code == 68 then
			move_right = true
		elseif key_code == 87 then
			move_up = true
		elseif key_code == 83 then
			move_down = true
		end
		--print(key_code)
	elseif type == KEY_UP_MESSAGE then
		local key_code = event.parameter1

		if key_code == 65 then
			move_left = false
		elseif key_code == 68 then
			move_right = false
		elseif key_code == 87 then
			move_up = false
		elseif key_code == 83 then
			move_down = false
		end
		--print(key_code)
	elseif type == KILLFOCUS_MESSAGE then
		move_left  = false
		move_right = false
		move_up    = false
		move_down  = false
		print('kill focus')
	else
		--print(events[i].type)
	end
end

if move_left then
	wave_x = wave_x - 0.01
end

if move_right then
	wave_x = wave_x + 0.01
end

if move_up then
	wave_y = wave_y + 0.01
end

if move_down then
	wave_y = wave_y - 0.01
end

push_transform()
	translate3D(wave_x, wave_y, 0.1)
	scale3D(0.2, 0.2, 0.3)
	rotate3D(-70, 1, 0, 0)
	wave:draw()
pop_transform()

push_transform()
	translate3D(0, 0.1, 0.1)
	scale3D(0.5, 0.5, 0.5)
	rotate3D(angle, 0, 1, 0)
	rotate3D(angle, 1, 0, 0)
	monkey:draw()
pop_transform()

push_transform()
	translate3D(0, 0.5, 0.1)
	scale3D(0.5, 0.5, 0.5)
	rotate3D(angle, 0, 1, 0)
	monkey:draw()
pop_transform()

angle = angle + 1