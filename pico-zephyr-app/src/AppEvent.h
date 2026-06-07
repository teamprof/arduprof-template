/* Copyright 2026 teamprof.net@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#include <stdint.h>

enum AppEvent {
    EventNull = 0,

    EventSystem, // iParam=<SystemTriggerSource>

    // EventBle, // iParam=<BleTriggerSource>
};

enum SystemTriggerSource {
    SysNull = 0,
    SysSoftwareTimer, // lParam=xTimer:uint32_t
    SysLowBattery,

    SysUartRxRdy,      // lParam=number of bytes received in this chunk
    SysUartRxOverflow, // ring buffer was too full, some data may be lost

    // SysBleAdvertize,
    // SysBleConnected,    // uParam=err
    // SysBleDisconnected, // uParam=reason
    // SysBleCCChanged,    // uParam=reason
    // SysBleOnRead,       // uParam=size, lParam=content value in uint32_t format
    // SysBleOnWrite,      // uParam=size, lParam=content value in uint32_t format

    SysEthUp,
    SysEthDown,
    SysEthIpv4Add,
};

// enum BleTriggerSource {
//     BleNull = 0,
//     BleConnected,    // uParam=err
//     BleDisconnected, // uParam=reason
//     BleOnRead,       // uParam=size, lParam=content value in uint32_t format
//     BleOnWrite,      // uParam=size, lParam=content value in uint32_t format
// };
