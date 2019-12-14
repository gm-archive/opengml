FNDEF8(tile_add, bg, left, top, width, height, x, y, depth)
FNDEF1(tile_delete, id)
FNDEF1(tile_exists, id)

FNDEF2(tile_set_alpha, id, alpha)
FNDEF2(tile_set_blend, id, b)
FNDEF3(tile_set_position, id, x, y)
FNDEF5(tile_set_region, id, left, top, width, height)
FNDEF2(tile_set_background, id, bg)
FNDEF2(tile_set_visible, id, v)
FNDEF2(tile_set_depth, id, d)
FNDEF3(tile_set_scale, id, sx, sy)

FNDEF1(tile_get_background, id)
FNDEF1(tile_get_depth, id)
FNDEF1(tile_get_width, id)
FNDEF1(tile_get_height, id)
FNDEF1(tile_get_left, id)
FNDEF1(tile_get_top, id)
FNDEF1(tile_get_visible, id)
FNDEF1(tile_get_x, id)
FNDEF1(tile_get_y, id)
FNDEF1(tile_get_xscale, id)
FNDEF1(tile_get_yscale, id)
FNDEF1(tile_get_blend, id)
FNDEF1(tile_get_alpha, id)
FNDEF1(tile_get_count, id)
FNDEF1(tile_get_ids_at_depth, depth)
FNDEF0(tile_get_ids)

FNDEF3(tile_layer_find, depth, x, y)
FNDEF3(tile_layer_delete_at, depth, x, y)
FNDEF1(tile_layer_delete, depth)
FNDEF1(tile_layer_hide, depth)
FNDEF1(tile_layer_show, depth)
FNDEF3(tile_layer_shift, depth, x, y)
FNDEF2(tile_layer_depth, depth, depthdst)