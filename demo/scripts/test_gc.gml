// ensure number of deletions by GC is as expected.

if (!ogm_garbage_collector_enabled()) exit;

ogm_assert(ogm_garbage_collector_process() == 0);

var a = 4;

ogm_assert(ogm_garbage_collector_process() == 0);

var arr = [1, 2, 3, 4];
arr = 0;

ogm_assert(ogm_garbage_collector_process() == 1);
ogm_assert(ogm_garbage_collector_process() == 0);

// instance gc test
instance_arr = [1, 2, 3, 4];

ogm_assert(ogm_garbage_collector_process() == 0);

instance_arr = 0;

ogm_assert(ogm_garbage_collector_process() == 1);
ogm_assert(ogm_garbage_collector_process() == 0);

// global gc test

global.global_arr = [1, 2, 3];

ogm_assert(ogm_garbage_collector_process() == 0);

global.global_arr = 1;

ogm_assert(ogm_garbage_collector_process() == 1);
ogm_assert(ogm_garbage_collector_process() == 0);

// more complicated scenario

var arr2 = [1, 2, 3]
instance_arr2 = arr2;
ogm_assert(ogm_garbage_collector_process() == 0);
var arr3 = arr2;
ogm_assert(ogm_garbage_collector_process() == 0);
arr2 = 0;
ogm_assert(ogm_garbage_collector_process() == 0);
ogm_assert(ogm_garbage_collector_process() == 0);
arr3 = 0;
ogm_assert(ogm_garbage_collector_process() == 0);
var arr4 = instance_arr2
ogm_assert(ogm_garbage_collector_process() == 0);
var arr5 = instance_arr2;
ogm_assert(ogm_garbage_collector_process() == 0);
instance_arr3 = instance_arr2;
ogm_assert(ogm_garbage_collector_process() == 0);
arr4 = 0;
ogm_assert(ogm_garbage_collector_process() == 0);
arr5 = 0;
ogm_assert(ogm_garbage_collector_process() == 0);
instance_arr2 = 0;
ogm_assert(ogm_garbage_collector_process() == 0);
instance_arr3 = 0;
ogm_assert(ogm_garbage_collector_process() == 1);

// circular reference
instance_arr20 = [0, 1, 2]
instance_arr20[@ 0] = instance_arr20;
ogm_assert(ogm_garbage_collector_process() == 0)

instance_arr20 = 0
ogm_assert(ogm_garbage_collector_process() == 1)

// CR 2
var instance_arr30 = [0, 1, 9]
var instance_arr31 = [1, 2, 7]
var instance_arr32 = [2, 3, 5]
instance_arr30[@0 ] = instance_arr31
instance_arr31[@0 ] = instance_arr32
instance_arr32[@0 ] = instance_arr30

root_ = instance_arr30

instance_arr30 = 0;
instance_arr31 = 0;
instance_arr32 = 0;

ogm_assert(ogm_garbage_collector_process() == 0)

root_ = 0

ogm_assert(ogm_garbage_collector_process() == 3)
ogm_assert(ogm_garbage_collector_process() == 0)

// implicit arrays

var arr40;
arr40[10] = 0;
arr40 = 0;
ogm_assert(ogm_garbage_collector_process() == 1)
ogm_assert(ogm_garbage_collector_process() == 0)

global.arr41[10] = 0;
ogm_assert(ogm_garbage_collector_process() == 0)

instance_arr42[10] = 0;
ogm_assert(ogm_garbage_collector_process() == 0)

show_debug_message("GC test complete.")
