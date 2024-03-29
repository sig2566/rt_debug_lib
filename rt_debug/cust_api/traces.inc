//Define traces points: DEF_TRACE(group_name, trace_name, format string)
DEF_TRACE(INIT_GROUP, CREATE_THREAD, "New thread was created num= %x thread_id = 0x%X")
DEF_TRACE(GEN_GROUP, PASSED, "Passed val= 0x%X")
DEF_TRACE(GEN_GROUP, HW_WRITE, "WRITE addr = 0x%X val = 0x%X")
DEF_TRACE(GEN_GROUP, HW_READ, "READ addr = 0x%X val = 0x%X")
DEF_TRACE(GEN_GROUP, NEW_THREAD, "Create thread num = 0x%X thread_id = 0x%X")
