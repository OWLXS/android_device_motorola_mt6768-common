/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.mediatek.hardware.mtkpower;

/**
 * Callback half of MediaTek's power service.
 *
 * Reconstructed from the vendor client blob (see IMtkPowerService for how and
 * why). Every method here is oneway, which is why the interface itself is.
 * Method order is load-bearing: it fixes the transaction codes.
 */
@VintfStability
oneway interface IMtkPowerCallback {
    /* code 1 */
    void mtkPowerHint(int hint, int data);

    /* code 2 */
    void notifyAppState(in String pack, in String act, int pid, int state, int uid);

    /* code 3 */
    void notifyScnUpdate(int scenario, int data);
}
