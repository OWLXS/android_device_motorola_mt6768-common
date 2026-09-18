/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "mtkpower-stub"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <aidl/vendor/mediatek/hardware/mtkpower/BnMtkPowerService.h>

namespace aidl::vendor::mediatek::hardware::mtkpower {

/**
 * Answers MediaTek's power service without doing anything.
 *
 * The point is only to *exist*. libcam.hal3a.v3.so asks servicemanager for this
 * interface on every still capture and, finding nobody, sits in libbinder's
 * service wait for a flat 5 seconds before continuing without it. Registering
 * here turns that into an immediate lookup.
 *
 * The perf hints themselves are deliberately dropped: honouring them would mean
 * driving the CPU/GPU governors, which on this ROM is the LineageOS libperfmgr
 * power HAL's job. This stub stays out of that so the two cannot fight over the
 * same knobs. Callers already handle the service doing nothing useful, since
 * that is exactly what they got when it was missing.
 */
class MtkPowerService : public BnMtkPowerService {
    ndk::ScopedAStatus perfCusLockHint(int32_t, int32_t, int32_t,
                                       int32_t* _aidl_return) override {
        // A perf lock handle. 0 reads as "no lock taken", which is honest here
        // and is what the callers see when the acquire fails.
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus perfLockAcquire(int32_t, int32_t, const std::vector<int32_t>&, int32_t,
                                       int32_t, int32_t* _aidl_return) override {
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus perfLockRelease(int32_t, int32_t) override {
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus perfLockReleaseSync(int32_t, int32_t, int32_t* _aidl_return) override {
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus mtkCusPowerHint(int32_t, int32_t) override {
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus mtkPowerHint(int32_t, int32_t) override {
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus querySysInfo(int32_t, int32_t, int32_t* _aidl_return) override {
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus setMtkPowerCallback(const std::shared_ptr<IMtkPowerCallback>&,
                                           int32_t* _aidl_return) override {
        // The callback is intentionally not retained: nothing here would ever
        // fire it, and holding it would only keep the caller alive.
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus setMtkScnUpdateCallback(int32_t, const std::shared_ptr<IMtkPowerCallback>&,
                                               int32_t* _aidl_return) override {
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus setSysInfo(int32_t, const std::string&, int32_t* _aidl_return) override {
        *_aidl_return = 0;
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus setSysInfoAsync(int32_t, const std::string&) override {
        return ndk::ScopedAStatus::ok();
    }
};

}  // namespace aidl::vendor::mediatek::hardware::mtkpower

int main() {
    using ::aidl::vendor::mediatek::hardware::mtkpower::MtkPowerService;

    auto service = ndk::SharedRefBase::make<MtkPowerService>();
    const std::string name = std::string(MtkPowerService::descriptor) + "/default";

    binder_status_t status =
            AServiceManager_addService(service->asBinder().get(), name.c_str());
    if (status != STATUS_OK) {
        LOG(ERROR) << "failed to register " << name << ": " << status;
        return 1;
    }

    LOG(INFO) << "registered " << name;

    // One thread is plenty: every call returns without doing work.
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_joinThreadPool();

    LOG(ERROR) << "binder thread pool returned, exiting";
    return 1;
}
