#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/input.h>
#include <linux/input/mt.h>

static struct input_dev *input_dev;
struct list_head *input_dev_list;
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
static kallsyms_lookup_name_t kallsyms_lookup_name_ptr;

typedef struct _UpTouch {
    int x;
    int y;
    bool isdown;
} UpTouch, *PUpTouch;

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name",
};

static int simulated_touch_slot = -1;

static int find_free_slot(struct input_dev *input_dev) {
    int slot;
    for (slot = 0; slot < input_dev->mt->num_slots; slot++) {
        if (!input_mt_is_active(&input_dev->mt->slots[slot])) {
            return slot+1;
        }
    }
    return -1; // No free slot found
}
void simulate_touch_event(struct input_dev *input_dev, int x, int y) {
 
    if(simulated_touch_slot<0){
    simulated_touch_slot = find_free_slot(input_dev);
    if (simulated_touch_slot < 0) {
        return;
    }
    }
    mutex_lock(&input_dev->mutex);
    input_event(input_dev, EV_ABS, ABS_MT_SLOT, simulated_touch_slot);
    input_mt_report_slot_state(input_dev, MT_TOOL_FINGER, 1);
    input_event(input_dev, EV_KEY, BTN_TOUCH, 1);
    input_event(input_dev, EV_ABS, ABS_MT_POSITION_X, x);
    input_event(input_dev, EV_ABS, ABS_MT_POSITION_Y, y);
    input_event(input_dev, EV_ABS, ABS_MT_PRESSURE, 30); // 使用更高的压力值
    input_event(input_dev, EV_ABS, ABS_MT_TOUCH_MAJOR, 30);
    input_event(input_dev, EV_ABS, ABS_MT_TOUCH_MINOR, 30);
    input_mt_sync_frame(input_dev);
    input_event(input_dev, EV_SYN, SYN_REPORT, 0);
    mutex_unlock(&input_dev->mutex);
}
void release_simulated_touch(struct input_dev *input_dev) {
    if (simulated_touch_slot < 0) {
        return;
    }
   
    mutex_lock(&input_dev->mutex);
    input_event(input_dev, EV_ABS, ABS_MT_SLOT, simulated_touch_slot);
    input_mt_report_slot_state(input_dev, MT_TOOL_FINGER, 0);
    input_event(input_dev, EV_KEY, BTN_TOUCH, 0);
    input_mt_sync_frame(input_dev);
    input_event(input_dev, EV_SYN, SYN_REPORT, 0);
    mutex_unlock(&input_dev->mutex);
    simulated_touch_slot = -1; // 重置触控 ID
}



