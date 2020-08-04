
//
//  Agora SDK
//  Created by Zheng, Ender in 2019-12.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)
#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "api2/NGIAgoraMediaNodeFactory.h"

namespace agora {
namespace rtc {

class IMediaNodeProvider : public RefCountInterface {
 public:
  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* name) = 0;
  virtual agora_refptr<IVideoFilter> createVideoFilter(const char* name) = 0;
  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* name) = 0;

 protected:
  ~IMediaNodeProvider() {}
};

}  // namespace rtc
}  // namespace agora
