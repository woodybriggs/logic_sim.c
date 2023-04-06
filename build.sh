cc -Iinclude -g tests/bucket.c -o build/tests/bucket
cc -Iinclude -g tests/hash_table.c -o build/tests/hash_table
cc -Iinclude -g tests/uint64_list.c -o build/tests/uint64_list

cc -Iinclude -g logic_sim.c -o build/logic_sim/logic_sim