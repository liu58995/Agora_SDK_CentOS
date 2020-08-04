//
//  Agora SDK
//
//  Created by Zheng Ender in 2019-11.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//
#pragma once
#include "AgoraRefPtr.h"
#include "api2/NGIAgoraMediaNodeFactory.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct agora_extension {
  agora::agora_refptr<agora::rtc::IVideoFilter> (*create_video_filter)();
  agora::agora_refptr<agora::rtc::IVideoRenderer> (*create_video_renderer)();
};

struct agora_core_capabilities {
  bool has_audio;
  bool has_video;
  // TODO(Bob):
  // more
};

//
// iOS does not allow us to load/unload dylib in runtime
// macOS doses not encourage us to load/unload dylib in runtime
// We will *NOT* provide "extension" capability in those platforms
// Third-party module vendors have to call register / unregister
// by themselves
//

#if defined(__APPLE__)

AGORA_API int AGORA_CALL agoraRegisterExtension(const char* name, agora_extension* ext);

AGORA_API int AGORA_CALL agoraUnregisterExtension(const char* name);

#else

struct agora_core {
  int (*register_extension)(const char* name, agora_extension* ext);
  int (*unregister_extension)(const char* name);
  int (*get_version)();
  int (*get_capabilities)(agora_core_capabilities* caps);
  int (*write_log)(const char* buf);
};

#endif

#if defined(__cplusplus)
};
#endif
