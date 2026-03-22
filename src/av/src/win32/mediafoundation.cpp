///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup av
/// @{


#include "icy/av/win32/mediafoundation.h"
#include "icy/logger.h"
#include "icy/platform.h"

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <audioclient.h>
#include <comdef.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")


namespace icy {
namespace av {


/// RAII initializer for Windows COM runtime
struct ComInit
{
    HRESULT hr;
    ComInit() : hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED)) {}
    ~ComInit() { if (SUCCEEDED(hr)) CoUninitialize(); }
    bool ok() const { return SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE; }
};

/// RAII initializer for Microsoft Media Foundation
struct MFInit
{
    HRESULT hr;
    MFInit() : hr(MFStartup(MF_VERSION)) {}
    ~MFInit() { if (SUCCEEDED(hr)) MFShutdown(); }
    bool ok() const { return SUCCEEDED(hr); }
};


// Helper: extract video capabilities from an IMFMediaSource
static void queryVideoCapabilities(IMFActivate* activate, Device& device)
{
    IMFMediaSource* source = nullptr;
    if (FAILED(activate->ActivateObject(IID_PPV_ARGS(&source))))
        return;

    IMFSourceReader* reader = nullptr;
    if (FAILED(MFCreateSourceReaderFromMediaSource(source, nullptr, &reader))) {
        source->Release();
        return;
    }

    // Enumerate all native media types for the first video stream
    DWORD typeIndex = 0;
    IMFMediaType* mediaType = nullptr;
    while (SUCCEEDED(reader->GetNativeMediaType(
               MF_SOURCE_READER_FIRST_VIDEO_STREAM, typeIndex, &mediaType))) {

        UINT32 width = 0, height = 0;
        if (SUCCEEDED(MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &width, &height))) {
            Device::VideoCapability cap;
            cap.width = static_cast<int>(width);
            cap.height = static_cast<int>(height);

            // Frame rate
            UINT32 fpsNum = 0, fpsDen = 1;
            if (SUCCEEDED(MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE, &fpsNum, &fpsDen)) && fpsDen > 0) {
                cap.maxFps = static_cast<double>(fpsNum) / fpsDen;
                cap.minFps = cap.maxFps; // MF doesn't expose min directly
            }

            // Frame rate range if available
            UINT32 minNum = 0, minDen = 1;
            if (SUCCEEDED(MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE_RANGE_MIN, &minNum, &minDen)) && minDen > 0) {
                cap.minFps = static_cast<double>(minNum) / minDen;
            }

            // Pixel format from subtype GUID
            GUID subtype = GUID_NULL;
            if (SUCCEEDED(mediaType->GetGUID(MF_MT_SUBTYPE, &subtype))) {
                if (subtype == MFVideoFormat_NV12) cap.pixelFormat = "nv12";
                else if (subtype == MFVideoFormat_YUY2) cap.pixelFormat = "yuyv422";
                else if (subtype == MFVideoFormat_MJPG) cap.pixelFormat = "mjpeg";
                else if (subtype == MFVideoFormat_RGB24) cap.pixelFormat = "bgr24";
                else if (subtype == MFVideoFormat_RGB32) cap.pixelFormat = "bgra";
                else if (subtype == MFVideoFormat_I420) cap.pixelFormat = "yuv420p";
                else if (subtype == MFVideoFormat_IYUV) cap.pixelFormat = "yuv420p";
                else cap.pixelFormat = "unknown";
            }

            // Avoid duplicate entries (same resolution + fps + format)
            bool dup = false;
            for (auto& existing : device.videoCapabilities) {
                if (existing.width == cap.width && existing.height == cap.height
                    && existing.maxFps == cap.maxFps && existing.pixelFormat == cap.pixelFormat) {
                    dup = true;
                    break;
                }
            }
            if (!dup)
                device.videoCapabilities.push_back(std::move(cap));
        }

        mediaType->Release();
        typeIndex++;
    }

    reader->Release();
    source->Shutdown();
    source->Release();
}


namespace mediafoundation {


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    if (type != Device::VideoInput && type != Device::AudioInput)
        return false;

    ComInit com;
    if (!com.ok()) {
        LError("COM initialization failed");
        return false;
    }

    MFInit mf;
    if (!mf.ok()) {
        LError("MFStartup failed, hr=", mf.hr);
        return false;
    }

    IMFAttributes* attrs = nullptr;
    HRESULT hr = MFCreateAttributes(&attrs, 1);
    if (FAILED(hr))
        return false;

    // Set the device type filter
    const GUID& sourceType = (type == Device::VideoInput)
        ? MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        : MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID;

    attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, sourceType);

    IMFActivate** ppDevices = nullptr;
    UINT32 count = 0;
    hr = MFEnumDeviceSources(attrs, &ppDevices, &count);
    attrs->Release();

    if (FAILED(hr) || count == 0)
        return false;

    for (UINT32 i = 0; i < count; i++) {
        WCHAR* friendlyName = nullptr;
        UINT32 nameLen = 0;
        ppDevices[i]->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendlyName, &nameLen);

        std::string devName = friendlyName ? icy::toUtf8(friendlyName) : "Unknown";

        // FFmpeg dshow uses friendly names as device identifiers:
        //   video=<FriendlyName> or audio=<FriendlyName>
        std::string prefix = (type == Device::VideoInput) ? "video=" : "audio=";
        std::string devId = prefix + devName;

        Device device(type, devId, devName, i == 0);

        // Query video capabilities
        if (type == Device::VideoInput) {
            queryVideoCapabilities(ppDevices[i], device);
        }

        devices.push_back(std::move(device));

        if (friendlyName) CoTaskMemFree(friendlyName);
        ppDevices[i]->Release();
    }

    CoTaskMemFree(ppDevices);
    return !devices.empty();
}


} // namespace mediafoundation


namespace wasapi {


bool getDeviceList(Device::Type type, std::vector<av::Device>& devices)
{
    if (type != Device::AudioInput && type != Device::AudioOutput)
        return false;

    ComInit com;
    if (!com.ok()) {
        LError("COM initialization failed");
        return false;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
    if (FAILED(hr))
        return false;

    EDataFlow flow = (type == Device::AudioOutput) ? eRender : eCapture;

    IMMDeviceCollection* collection = nullptr;
    hr = enumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &collection);
    if (FAILED(hr)) {
        enumerator->Release();
        return false;
    }

    // Get default device ID for comparison
    std::string defaultId;
    {
        IMMDevice* defaultDev = nullptr;
        ERole role = eMultimedia;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(flow, role, &defaultDev))) {
            LPWSTR id = nullptr;
            if (SUCCEEDED(defaultDev->GetId(&id))) {
                defaultId = icy::toUtf8(id);
                CoTaskMemFree(id);
            }
            defaultDev->Release();
        }
    }

    UINT count = 0;
    collection->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IMMDevice* endpoint = nullptr;
        if (FAILED(collection->Item(i, &endpoint)))
            continue;

        // Get device ID
        LPWSTR pwszId = nullptr;
        endpoint->GetId(&pwszId);
        std::string devId = pwszId ? icy::toUtf8(pwszId) : "";
        if (pwszId) CoTaskMemFree(pwszId);

        // Get friendly name
        IPropertyStore* props = nullptr;
        std::string devName = "Unknown";
        if (SUCCEEDED(endpoint->OpenPropertyStore(STGM_READ, &props))) {
            PROPVARIANT varName;
            PropVariantInit(&varName);
            if (SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &varName))) {
                if (varName.vt == VT_LPWSTR)
                    devName = icy::toUtf8(varName.pwszVal);
            }
            PropVariantClear(&varName);
            props->Release();
        }

        bool isDefault = (devId == defaultId);
        Device device(type, devId, devName, isDefault);

        // Query audio capabilities via IAudioClient
        IAudioClient* audioClient = nullptr;
        if (SUCCEEDED(endpoint->Activate(
                __uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&audioClient))) {
            WAVEFORMATEX* mixFormat = nullptr;
            if (SUCCEEDED(audioClient->GetMixFormat(&mixFormat))) {
                Device::AudioCapability cap;
                cap.sampleRate = static_cast<int>(mixFormat->nSamplesPerSec);
                cap.channels = static_cast<int>(mixFormat->nChannels);

                // Map WAVE format to FFmpeg sample format name
                if (mixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
                    (mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                     mixFormat->wBitsPerSample == 32)) {
                    cap.sampleFormat = "flt";
                } else if (mixFormat->wBitsPerSample == 16) {
                    cap.sampleFormat = "s16";
                } else if (mixFormat->wBitsPerSample == 32) {
                    cap.sampleFormat = "s32";
                } else {
                    cap.sampleFormat = "s16";
                }

                device.audioCapabilities.push_back(std::move(cap));
                CoTaskMemFree(mixFormat);
            }
            audioClient->Release();
        }

        devices.push_back(std::move(device));
        endpoint->Release();
    }

    collection->Release();
    enumerator->Release();

    return !devices.empty();
}


} // namespace wasapi


//
// Windows Device Watcher
//

/// Internal implementation for WindowsDeviceWatcher
struct WindowsDeviceWatcher::Impl
{
    /// Internal COM notification handler for WindowsDeviceWatcher
    class NotificationClient : public IMMNotificationClient
    {
    public:
        NotificationClient(DeviceManager* mgr) : _mgr(mgr), _refCount(1) {}

        // IUnknown
        ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&_refCount); }
        ULONG STDMETHODCALLTYPE Release() override {
            ULONG ref = InterlockedDecrement(&_refCount);
            if (ref == 0) delete this;
            return ref;
        }
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == __uuidof(IMMNotificationClient)) {
                *ppv = static_cast<IMMNotificationClient*>(this);
                AddRef();
                return S_OK;
            }
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        // IMMNotificationClient
        HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
            _mgr->DevicesChanged.emit();
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
            _mgr->DevicesChanged.emit();
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
            _mgr->DevicesChanged.emit();
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override {
            _mgr->DevicesChanged.emit();
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
            return S_OK;
        }

    private:
        DeviceManager* _mgr;
        LONG _refCount;
    };

    IMMDeviceEnumerator* enumerator{nullptr};
    NotificationClient* client{nullptr};
    bool active{false};
};


WindowsDeviceWatcher::WindowsDeviceWatcher(DeviceManager* manager)
    : DeviceWatcher(manager)
    , _impl(std::make_unique<Impl>())
    , _manager(manager)
{
}


WindowsDeviceWatcher::~WindowsDeviceWatcher() noexcept
{
    stop();
}


bool WindowsDeviceWatcher::start()
{
    if (_impl->active)
        return true;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&_impl->enumerator);
    if (FAILED(hr))
        return false;

    _impl->client = new Impl::NotificationClient(_manager);
    hr = _impl->enumerator->RegisterEndpointNotificationCallback(_impl->client);
    if (FAILED(hr)) {
        _impl->client->Release();
        _impl->client = nullptr;
        _impl->enumerator->Release();
        _impl->enumerator = nullptr;
        return false;
    }

    _impl->active = true;
    return true;
}


void WindowsDeviceWatcher::stop()
{
    if (!_impl->active)
        return;

    if (_impl->enumerator && _impl->client) {
        _impl->enumerator->UnregisterEndpointNotificationCallback(_impl->client);
        _impl->client->Release();
        _impl->client = nullptr;
    }
    if (_impl->enumerator) {
        _impl->enumerator->Release();
        _impl->enumerator = nullptr;
    }

    _impl->active = false;
}


} // namespace av
} // namespace icy


/// @\}
