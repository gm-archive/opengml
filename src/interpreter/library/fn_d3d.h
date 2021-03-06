FNDEF4(d3d_set_fog, enable, colour, start, end)

FNDEF0(d3d_start)
FNDEF0(d3d_end)
FNDEF1(d3d_set_hidden, e)
FNDEF1(d3d_set_culling, e)
FNDEF1(d3d_set_zwriteenable, e)

FNDEF9(d3d_set_projection, xsrc, ysrc, zsrc, xdst, ydst, zdst, xup, yup, zup)
FNDEF13(d3d_set_projection_ext, xsrc, ysrc, zsrc, xdst, ydst, zdst, xup, yup, zup, fovangle, ratio, near, far)
FNDEF5(d3d_set_projection_ortho, x, y, w, h, angle)

FNDEF1(d3d_primitive_begin, type)
FNDEF3(d3d_vertex, x, y, z)
FNDEF5(d3d_vertex_colour, x, y, z, col, alpha)
ALIAS(d3d_vertex_colour, d3d_vertex_color)
FNDEF2(d3d_primitive_begin_texture, type, tex)
FNDEF5(d3d_vertex_texture, x, y, z, u, v)
FNDEF7(d3d_vertex_texture_colour, x, y, z, u, v, col, alpha)
ALIAS(d3d_vertex_texture_colour, d3d_vertex_texture_color)
FNDEF0(d3d_primitive_end)

FNDEF0(d3d_transform_set_identity)
FNDEF3(d3d_transform_set_translation, x, y, z)
FNDEF3(d3d_transform_set_scaling, x, y, z)
FNDEF1(d3d_transform_set_rotation_x, theta)
FNDEF1(d3d_transform_set_rotation_y, theta)
FNDEF1(d3d_transform_set_rotation_z, theta)
FNDEF4(d3d_transform_set_rotation_axis, x, y, z, a)

FNDEF3(d3d_transform_add_translation, x, y, z)
FNDEF3(d3d_transform_add_scaling, x, y, z)
FNDEF1(d3d_transform_add_rotation_x, theta)
FNDEF1(d3d_transform_add_rotation_y, theta)
FNDEF1(d3d_transform_add_rotation_z, theta)
FNDEF4(d3d_transform_add_rotation_axis, x, y, z, a)

FNDEF3(d3d_transform_vertex, x, y, z)

// ogm-specific
FNDEF3(d3d_transform_vertex_model_view, x, y, z)
FNDEF3(d3d_transform_vertex_model_view_projection, x, y, z)

FNDEF0(d3d_transform_stack_clear)
FNDEF0(d3d_transform_stack_empty)
FNDEF0(d3d_transform_stack_push)
FNDEF0(d3d_transform_stack_pop)
FNDEF0(d3d_transform_stack_top)
FNDEF0(d3d_transform_stack_discard)

FNDEF9(d3d_draw_floor, x1, y1, z1, x2, y2, z2, tex, hrepeat, vrepeat)
FNDEF9(d3d_draw_wall, x1, y1, z1, x2, y2, z2, tex, hrepeat, vrepeat)
FNDEF9(d3d_draw_block, x1, y1, z1, x2, y2, z2, tex, hrepeat, vrepeat)