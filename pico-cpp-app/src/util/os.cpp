/* Copyright 2026 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "os.h"
#include "../ArduProfApp.h"

#if defined ARDUINO_ARCH_RP2040 || defined ARDUPROF_PICO_C
#include <malloc.h>
#include <unistd.h> 
#include "pico/stdlib.h"

extern "C" {
    extern char __StackLimit; 
}

// #elif defined(ARDUPROF_FREERTOS)
// #include <malloc.h>
// #include "FreeRTOS.h"
// #include "task.h"

#elif defined(ARDUPROF_MBED)
#include "rtos.h"
#include "mbed_stats.h"

#endif


namespace os {

    int getRunningCore(void)
    {
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUPROF_PICO_C) || defined(ARDUPROF_MBED)
        return get_core_num();
#elif defined ESP_PLATFORM
        return xPortGetCoreID();
#else
        return -1;
#endif    
    }

    int getPriority(void)
    {
#if defined ARDUPROF_FREERTOS
        return uxTaskPriorityGet(NULL);
#elif defined(ARDUPROF_MBED)
        osThreadId_t current_thread_id = rtos::ThisThread::get_id();
        osPriority_t priority = osThreadGetPriority(current_thread_id);
        return priority;
#else
        return -1;
#endif
    }

    uint32_t getFreeHeapSize(void)
    {
#if defined(ARDUPROF_FREERTOS)
#if defined(ARDUPROF_PICO_C)
        // Get stats from the standard allocator
        struct mallinfo mi = mallinfo();
        
        // mi.uordblks is the total space currently allocated by malloc
        // mi.fordblks is the free space inside the already cleared/held arena
        uint32_t total_allocated = mi.uordblks;
        uint32_t free_in_arena = mi.fordblks;
        
        // Find the current system break point (the top boundary of the active heap area)
        char *heap_top = (char *)sbrk(0);
        
        // Calculate the unallocated gap between the current top of the heap and the stack limit
        uint32_t unallocated_gap = &__StackLimit - heap_top;
        
        // Total free heap is the unallocated pool plus any freed blocks inside the active arena
        return unallocated_gap + free_in_arena;
#else
        return xPortGetFreeHeapSize();
#endif        
#elif defined(ARDUPROF_MBED)
        mbed_stats_heap_t heap_stats;
        mbed_stats_heap_get(&heap_stats);
        uint32_t free_heap = heap_stats.reserved_size - heap_stats.current_size;
        return free_heap;
#else
        return 0;
#endif
    }

};