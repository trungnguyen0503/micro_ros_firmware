#ifndef GLOBAL_H
#define GLOBAL_H

#include "cmsis_os.h"
#include <rclc/rclc.h>

extern rcl_allocator_t g_rcl_allocator;
extern rclc_support_t g_rclc_support;
extern rcl_node_t g_rcl_node;
extern rcl_node_t g_rcl_node2;
extern osMutexId_t g_mutex;
extern rcl_ret_t g_return_code;
extern rcl_ret_t g_return_code2;

#endif