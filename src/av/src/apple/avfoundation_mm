// support GCC compiler
#ifndef __has_feature
#define __has_feature(x) 0
#endif

#include "scy/logger.h"
#include "scy/av/devicemanager.h"

#import <assert.h>
#import <AVFoundation/AVFoundation.h>


namespace scy {
namespace av {
namespace avfoundation {


bool GetAVFoundationVideoDevices(Device::Type type, std::vector<Device>* devices)
{
#if !__has_feature(objc_arc)
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#else
  @autoreleasepool
#endif
  {
    NSArray* capture_devices = [AVCaptureDevice devices];
    LDebug([capture_devices count], " capture device(s) found:")
    for (AVCaptureDevice* capture_device in capture_devices) {
      if ((type == Device::VideoInput && [capture_device hasMediaType:AVMediaTypeVideo]) ||
          (type == Device::AudioInput && [capture_device hasMediaType:AVMediaTypeAudio])) {
      // if ([capture_device hasMediaType:AVMediaTypeVideo] ||
      //     [capture_device hasMediaType:AVMediaTypeMuxed]) {
        static NSString* const kFormat = @"localizedName: \"%@\", "
            @"modelID: \"%@\", uniqueID \"%@\", isConnected: %d, "
            @"isInUseByAnotherApplication: %d";
        NSString* info = [NSString
            stringWithFormat:kFormat,
                             [capture_device localizedName],
                             [capture_device modelID],
                             [capture_device uniqueID],
                             [capture_device isConnected],
                             [capture_device isInUseByAnotherApplication]];
        LDebug('\t', [info UTF8String])

        std::string uniqueID([[capture_device uniqueID] UTF8String]);
        std::string name([[capture_device localizedName] UTF8String]);
        devices->push_back(Device(type, uniqueID, name));
      }
    }
  }
#if !__has_feature(objc_arc)
  [pool drain];
#endif
  return true;
}


} // namespace avfoundation
} // namespace av
} // namespace scy


/*
 * libjingle
 * Copyright 2010, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
