///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "scy/av/win32/directshow.h"
#include "scy/logger.h"
#include "scy/platform.h"

#include <dbt.h> // DBT_* & DEV_*
#include <dshow.h>
#include <ks.h>
#include <ksmedia.h> // KSCATEGORY_*
#include <windows.h>




namespace scy {
namespace av {
namespace dshow {


bool getDeviceCategoryList(Device::Type type, REFGUID category, std::vector<av::Device>& devices)
{
    IMoniker* m = nullptr;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool needCoUninitialize = SUCCEEDED(hr);
    if (FAILED(hr)) {
        if (hr != RPC_E_CHANGED_MODE) {
            LError("CoInitialize failed, hr=", hr);
            return false;
        } else {
            LWarn("CoInitialize Changed Mode");
        }
    }

    ICreateDevEnum* devenum = nullptr;
    if (CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER,
                         IID_ICreateDevEnum, (void**)&devenum) != S_OK)
        return false;

    IEnumMoniker* classenum = nullptr;
    if (devenum->CreateClassEnumerator(category, (IEnumMoniker**)&classenum, 0) != S_OK)
        return false;

    while (classenum->Next(1, &m, nullptr) == S_OK) {
        VARIANT var;
        IPropertyBag* bag = nullptr;
        LPMALLOC coMalloc = nullptr;
        IBindCtx* bindCtx = nullptr;
        LPOLESTR olestr = nullptr;
        std::string devId, devHumanName;

        if (CoGetMalloc(1, &coMalloc) != S_OK)
            goto fail;
        if (CreateBindCtx(0, &bindCtx) != S_OK)
            goto fail;

        // Get an uuid for the device that we can pass to ffmpeg directly
        if (m->GetDisplayName(bindCtx, nullptr, &olestr) != S_OK)
            goto fail;
        devId = scy::toUtf8(olestr);

        // Replace ':' with '_' since FFmpeg uses : to delimitate sources
        for (size_t i = 0; i < devId.length(); i++)
            if (devId[i] == ':')
                devId[i] = '_';

        // Get a human friendly name/description
        if (m->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void**)&bag) != S_OK)
            goto fail;

        var.vt = VT_BSTR;
        if (bag->Read(L"FriendlyName", &var, nullptr) != S_OK)
            goto fail;
        devHumanName = scy::toUtf8(var.bstrVal);

        devices.push_back(av::Device(type, "video=" + devId, devHumanName));

    fail:
        if (olestr && coMalloc)
            coMalloc->Free(olestr);
        if (bindCtx)
            bindCtx->Release();
        if (bag)
            bag->Release();
        m->Release();
    }
    classenum->Release();

    if (needCoUninitialize) {
        CoUninitialize();
    }

    return !devices.empty();
}


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    switch (type) {
        case Device::VideoInput:
            return getDeviceCategoryList(type, CLSID_VideoInputDeviceCategory, devices);
        case Device::AudioInput:
            return getDeviceCategoryList(type, CLSID_AudioInputDeviceCategory, devices);
        default:
            // assert(0 && "unknown dshow device type");
            LDebug("DirectShow cannot enumerate output devices: Not implemented");
            break;
    }

    return false;
}


} // namespace dshow
} // namespace av
} // namespace scy


/// @\}
