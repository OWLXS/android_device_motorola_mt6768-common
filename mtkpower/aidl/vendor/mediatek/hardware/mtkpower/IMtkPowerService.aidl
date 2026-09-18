/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.mediatek.hardware.mtkpower;

import vendor.mediatek.hardware.mtkpower.IMtkPowerCallback;

/**
 * MediaTek's vendor power service.
 *
 * Stock firmware serves this from the same process as android.hardware.power;
 * this ROM uses the LineageOS libperfmgr power HAL instead, which implements
 * only the AOSP interface, so nothing answers here. That is not harmless:
 * libcam.hal3a.v3.so looks this service up on every still capture and
 * libbinder blocks for a flat 5 seconds before giving up, which is the bulk of
 * the shutter lag on this device.
 *
 * MediaTek does not publish this interface in source, and the tree only carries
 * the HIDL one (hardware/mediatek/interfaces/hardware/mtkpower), which splits
 * the same calls across IMtkPower and IMtkPerf and so cannot be used to derive
 * the AIDL layout. It was recovered instead from the vendor client blob
 * vendor.mediatek.hardware.mtkpower-V1-ndk.so:
 *
 *   - names and types, from the demangled BpMtkPowerService symbols
 *   - declaration order, from the transaction code each proxy method passes to
 *     AIBinder_transact (w1)
 *   - oneway-ness, from the flags argument (w4): 1 = FLAG_ONEWAY,
 *     0x10000000 = FLAG_CLEAR_BUF, i.e. a synchronous call
 *
 * So **do not reorder these methods and do not change oneway**: both are part
 * of the wire format the prebuilt client already speaks. The transaction code
 * each method must get is noted alongside it.
 */
@VintfStability
interface IMtkPowerService {
    /* code 1 */
    int perfCusLockHint(int hint, int duration, int reserved);

    /* code 2 */
    int perfLockAcquire(int handle, int duration, in int[] boostsList, int reserved,
            int reserved2);

    /* code 3 */
    oneway void perfLockRelease(int handle, int reserved);

    /* code 4 */
    int perfLockReleaseSync(int handle, int reserved);

    /* code 5 */
    oneway void mtkCusPowerHint(int hint, int data);

    /* code 6 */
    oneway void mtkPowerHint(int hint, int data);

    /* code 7 */
    int querySysInfo(int cmd, int param);

    /* code 8 */
    int setMtkPowerCallback(in IMtkPowerCallback callback);

    /* code 9 */
    int setMtkScnUpdateCallback(int scenario, in IMtkPowerCallback callback);

    /* code 10 */
    int setSysInfo(int type, in String data);

    /* code 11 */
    oneway void setSysInfoAsync(int type, in String data);
}
