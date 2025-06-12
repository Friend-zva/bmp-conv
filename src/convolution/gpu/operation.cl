__kernel void apply_filter(__global const uchar* data_source,
                           __global uchar* data_conv,
                           __global const float* matrix_f, int height,
                           int width, int height_f, int width_f, float factor,
                           float bias, __global float* debug_info) {
    int y = get_global_id(0);
    int x = get_global_id(1);

    if (y >= height || x >= width) {
        return;
    }

    float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f;

    int center_height_f = height_f / 2;
    int center_width_f = width_f / 2;
    unsigned int index_f = 0;
    for (int y_f = 0; y_f < height_f; y_f++) {
        for (int x_f = 0; x_f < width_f; x_f++) {
            int y_loc = (y - center_height_f + y_f + height) % height;
            int x_loc = (x - center_width_f + x_f + width) % width;

            int index = (y_loc * width + x_loc) * 3;
            int value = matrix_f[index_f];
            r_sum += (float)data_source[index] * value;
            g_sum += (float)data_source[index + 1] * value;
            b_sum += (float)data_source[index + 2] * value;
            index_f++;
        }
    }

    int index = (y * width + x) * 3;
    data_conv[index] = convert_uchar_sat(factor * r_sum + bias);
    data_conv[index + 1] = convert_uchar_sat(factor * g_sum + bias);
    data_conv[index + 2] = convert_uchar_sat(factor * b_sum + bias);
}
