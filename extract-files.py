#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/motorola/mt6768-common',
    'hardware/mediatek',
    'hardware/mediatek/libion_mtk',
    'hardware/mediatek/libmtkperf_client'
]

def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None


lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    (
        'vendor.mediatek.hardware.videotelephony-V1-ndk'
    ): lib_fixup_vendor_suffix,
}

blob_fixups: blob_fixups_user_type = {
    'vendor/etc/init/android.hardware.secure_element@1.2-service-mediatek.rc': blob_fixup()
        .regex_replace('class hal', 'class hal\n    disabled'),
    'system_ext/bin/hw/android.hardware.audio.parameter_parser.service': blob_fixup()
        .replace_needed('av-audio-types-aidl-V1-ndk.so', 'av-audio-types-aidl-V3-ndk.so'),
    'system_ext/lib64/libimsma.so': blob_fixup()
        .replace_needed('libsink.so', 'libsink-mtk.so'),
    'system_ext/lib64/libsink-mtk.so': blob_fixup()
        .add_needed('libaudioclient_shim.so'),
    'system_ext/priv-app/ImsService/ImsService.apk': blob_fixup()
        .apktool_patch('blob-patches/ImsService'),
    (
        'vendor/bin/hw/android.hardware.audio.service-aidl.mediatek',
        'vendor/lib64/hw/android.hardware.soundtrigger3-impl.so',
    ): blob_fixup()
        .replace_needed('libaudio_aidl_conversion_common_ndk.so', 'libaudio_aidl_conversion_common_ndk_prebuilt.so'),
    'vendor/bin/mnld' : blob_fixup()
        .replace_needed('android.hardware.sensors-V2-ndk.so', 'android.hardware.sensors-V3-ndk.so')
        .replace_needed('libmnl.so', 'libmnl_mtk.so'),
    'vendor/etc/init/android.hardware.biometrics.fingerprint-service.rc': blob_fixup()
        .regex_replace('user root', 'user system')
        .regex_replace('group root', 'group system input uhid'),
    'vendor/lib64/android.hardware.audio.core-impl-mediatek.so': blob_fixup()
        .add_needed('libaudioutils_shim.so')
        .replace_needed('libaudio_aidl_conversion_common_ndk.so', 'libaudio_aidl_conversion_common_ndk_prebuilt.so'),
    'vendor/lib64/libaalservice.so' : blob_fixup()
        .replace_needed('android.hardware.sensors-V2-ndk.so', 'android.hardware.sensors-V3-ndk.so'),
    (
        'vendor/lib64/libcodec2_mtk_vdec.so',
        'vendor/lib64/libcodec2_mtk_venc.so'
    ): blob_fixup()
        .replace_needed('libformatter.so', 'libformatter_mtk.so'),
    (
        'vendor/bin/hw/android.hardware.graphics.allocator-V2-service-mediatek',
        'vendor/lib64/egl/libGLES_mali.so',
        'vendor/lib64/hw/android.hardware.graphics.allocator-V2-mediatek.so',
        'vendor/lib64/hw/mapper.mediatek.so',
        'vendor/lib64/libcodec2_fsr.so',
        'vendor/lib64/libgpud.so',
        'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V2-ndk.so',
        'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V4-ndk.so',
    ): blob_fixup()
        .replace_needed('android.hardware.graphics.common-V5-ndk.so', 'android.hardware.graphics.common-V7-ndk.so'),
    'vendor/lib64/vendor.mediatek.hardware.pq_aidl-V7-ndk.so': blob_fixup()
        .replace_needed('android.hardware.graphics.common-V4-ndk.so', 'android.hardware.graphics.common-V7-ndk.so'),
    (
        'vendor/lib64/hw/android.hardware.audio.effect.aidl-impl-mediatek.so',
        'vendor/lib64/hw/hwcomposer.mt6768.so',
        'vendor/lib64/hw/vendor.mediatek.hardware.pq_aidl-impl.so',
        'vendor/lib64/libpqxmlparser.so',
        'vendor/lib64/librt_extamp_intf.so',
        'vendor/lib64/libsilkybrightnesscore.so'
    ): blob_fixup()
        .replace_needed('libtinyxml2.so', 'libtinyxml2-v34.so'),
    'vendor/lib64/libutinterface_custom_md.so': blob_fixup()
        .add_needed('libutinterface_md.so'),
}  # fmt: skip

module = ExtractUtilsModule(
    'mt6768-common',
    'motorola',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
