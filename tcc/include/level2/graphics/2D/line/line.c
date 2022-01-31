void initialize_line (Line* line, N_16 x1, N_16 y1, N_16 x2, N_16 y2)
{
    line->x1 = x1;
    line->y1 = y1;
    line->x2 = x2;
    line->y2 = y2;
}


N_32 module_1D (Z_32 number)
{
    if(number < 0)
        return -number;

    return number;
}


void draw_line (N_32* screen, N_32 screen_width, N_32 screen_height, Line *line, N_32 color)
{
    N_32 length_x;
    N_32 length_y;
    N_16 x1;
    N_16 x2;
    N_16 y1;
    N_16 y2;

    x1 = line->x1;
    x2 = line->x2;
    y1 = line->y1;
    y2 = line->y2;

    length_x = module_1D(x2 - x1) + 1;
    length_y = module_1D(y2 - y1) + 1;

    if(length_x >= length_y)
    {
        if(y1 < y2)
        {
            if(x1 < x2)
            {
                N_32 x_tail_length;
                N_32 current_x;
                N_32 max_x;

                x_tail_length = x1 * length_y;
                current_x = x_tail_length;
                max_x = x2 * length_y;

                for(; y1 <= y2; ++y1)
                {
                    x_tail_length += length_x;

                    if(x_tail_length > max_x)
                        x_tail_length = max_x + length_y;

                    for(; current_x < x_tail_length; current_x += length_y, ++x1)
                        screen[(screen_height - y1) * screen_width + x1] = color;
                }
            }
            else
            {
                N_32 x_tail_length;
                N_32 current_x;
                N_32 max_x;

                x_tail_length = x2 * length_y;
                current_x = x_tail_length;
                max_x = x1 * length_y;

                for(; y2 >= y1; --y2)
                {
                    x_tail_length += length_x;

                    if(x_tail_length > max_x)
                        x_tail_length = max_x + length_y;

                    for(; current_x < x_tail_length; current_x += length_y, ++x2)
                        screen[(screen_height - y2) * screen_width + x2] = color;
                }
            }
        }
        else
        {
            if(x1 < x2)
            {
                N_32 x_tail_length;
                N_32 current_x;
                N_32 max_x;

                x_tail_length = x1 * length_y;
                current_x = x_tail_length;
                max_x = x2 * length_y;

                for(; y1 >= y2; --y1)
                {
                    x_tail_length += length_x;

                    if(x_tail_length > max_x)
                        x_tail_length = max_x + length_y;

                    for(; current_x < x_tail_length; current_x += length_y, ++x1)
                        screen[(screen_height - y1) * screen_width + x1] = color;
                }
            }
            else
            {
                N_32 x_tail_length;
                N_32 current_x;
                N_32 max_x;

                x_tail_length = x2 * length_y;
                current_x = x_tail_length;
                max_x = x1 * length_y;

                for(; y2 <= y1; ++y2)
                {
                    x_tail_length += length_x;

                    if(x_tail_length > max_x)
                        x_tail_length = max_x + length_y;

                    for(; current_x < x_tail_length; current_x += length_y, ++x2)
                        screen[(screen_height - y2) * screen_width + x2] = color;
                }
            }
        }
    }
    else
    {
        if(x1 < x2)
        {
            if(y1 < y2)
            {
                N_32 y_tail_length;
                N_32 current_y;
                N_32 max_y;

                y_tail_length = y1 * length_x;
                current_y = y_tail_length;
                max_y = y2 * length_x;

                for(; x1 <= x2; ++x1)
                {
                    y_tail_length += length_y;

                    if(y_tail_length > max_y)
                        y_tail_length = max_y + length_x;

                    for(; current_y < y_tail_length; current_y += length_x, ++y1)
                        screen[(screen_height - y1) * screen_width + x1] = color;
                }
            }
            else
            {
                N_32 y_tail_length;
                N_32 current_y;
                N_32 max_y;

                y_tail_length = y2 * length_x;
                current_y = y_tail_length;
                max_y = y1 * length_x;

                for(; x2 >= x1; --x2)
                {
                    y_tail_length += length_y;

                    if(y_tail_length > max_y)
                        y_tail_length = max_y + length_x;

                    for(; current_y < y_tail_length; current_y += length_x, ++y2)
                        screen[(screen_height - y2) * screen_width + x2] = color;
                }
            }
        }
        else
        {
            if(y1 < y2)
            {
                N_32 y_tail_length;
                N_32 current_y;
                N_32 max_y;

                y_tail_length = y1 * length_x;
                current_y = y_tail_length;
                max_y = y2 * length_x;

                for(; x1 >= x2; --x1)
                {
                    y_tail_length += length_y;

                    if(y_tail_length > max_y)
                        y_tail_length = max_y + length_x;

                    for(; current_y < y_tail_length; current_y += length_x, ++y1)
                        screen[(screen_height - y1) * screen_width + x1] = color;
                }
            }
            else
            {
                N_32 y_tail_length;
                N_32 current_y;
                N_32 max_y;

                y_tail_length = y2 * length_x;
                current_y = y_tail_length;
                max_y = y1 * length_x;

                for(; x2 <= x1; ++x2)
                {
                    y_tail_length += length_y;

                    if(y_tail_length > max_y)
                        y_tail_length = max_y + length_x;

                    for(; current_y < y_tail_length; current_y += length_x, ++y2)
                        screen[(screen_height - y2) * screen_width + x2] = color;
                }
            }
        }
    }
}