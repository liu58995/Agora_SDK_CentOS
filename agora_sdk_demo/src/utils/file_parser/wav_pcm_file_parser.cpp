//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-08.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#include "wav_pcm_file_parser.h"
#include <stdlib.h>
#include <string.h>

WavPcmFileParser::WavPcmFileParser(const char* filepath)
    : wavFilePath_(strdup(filepath)),
      wavFile_(nullptr),
      readedLength_(0),
      bufDataLength_(0),
      isEof_(false) {}

WavPcmFileParser::~WavPcmFileParser() {
  if (wavFile_) {
    fclose(wavFile_);
  }
  free(static_cast<void*>(wavFilePath_));
}

bool WavPcmFileParser::open() {
  wavFile_ = fopen(wavFilePath_, "rb");
  if (wavFile_ == nullptr) {
    printf("Open test file %s failed\n", wavFilePath_);
    return false;
  }
  unsigned char buffer[44] = {0};
  size_t readsize = fread(buffer, 1, sizeof(buffer), wavFile_);
  if (readsize < sizeof(buffer)) {
    printf("Read test file %s failed\n", wavFilePath_);
    return false;
  }
  parseWAVHeader(buffer, wavHeader_);

  // For WAV file generated by ffmpeg.
  if (strncmp(wavHeader_.dataTag, "LIST", 4) == 0) {
    printf("LIST trunk size %d\n", wavHeader_.dataLength);
    readsize = fread(buffer, 1, wavHeader_.dataLength, wavFile_);
    if (readsize < wavHeader_.dataLength) {
      printf("Invalid WAV audio file format %s\n", wavFilePath_);
      return false;
    }
    readsize = fread(buffer, 1, 8, wavFile_);
    if (readsize < 8) {
      printf("Invalid WAV audio file format %s\n", wavFilePath_);
      return false;
    }
    strncpy(reinterpret_cast<char*>(wavHeader_.dataTag), reinterpret_cast<const char*>(buffer), 4);
    wavHeader_.dataLength = get_le32(buffer + 4);
  }
  if (strncmp(wavHeader_.tag, "RIFF", 4) != 0 || strncmp(wavHeader_.typeTag, "WAVE", 4) != 0 ||
      strncmp(wavHeader_.fmtTag, "fmt ", 4) != 0 || strncmp(wavHeader_.dataTag, "data", 4) != 0) {
    printf("Unsupported test file format %s\n", wavFilePath_);
    wavHeader_.numberOfChannels = 0;
    wavHeader_.sampleRateHz = 0;
    return false;
  }
  return true;
}

bool WavPcmFileParser::hasNext() {
  if (wavHeader_.numberOfChannels == 0 || wavHeader_.sampleRateHz == 0) {
    return false;
  }
  readData();
  if (isEof_ || readedLength_ >= wavHeader_.dataLength) {
    return false;
  }
  return true;
}

int WavPcmFileParser::reset() {
  fseek(wavFile_, 0, SEEK_SET);
  readedLength_ = 0;
  isEof_ = false;
  return 0;
}

int WavPcmFileParser::getNumberOfChannels() { return wavHeader_.numberOfChannels; }
int WavPcmFileParser::getSampleRate() { return wavHeader_.sampleRateHz; }

int WavPcmFileParser::getBitsPerSample() { return wavHeader_.bitsPerSample; }

void WavPcmFileParser::getNext(char* buffer, int* length) {
  memcpy(buffer, dataBuffer_, bufDataLength_);
  *length = bufDataLength_;
  bufDataLength_ = 0;
}

agora::rtc::AUDIO_CODEC_TYPE WavPcmFileParser::getCodecType() {
  return agora::rtc::AUDIO_CODEC_PCMU;
}

int WavPcmFileParser::getSampleRateHz() { return getSampleRate(); }

void WavPcmFileParser::readData() {
  if (isEof_ || readedLength_ >= wavHeader_.dataLength) {
    return;
  }
  int length =
      wavHeader_.sampleRateHz / 100 * wavHeader_.numberOfChannels * wavHeader_.bitsPerSample / 8;
  if (bufDataLength_ > 0) {
    return;
  }
  size_t readsize = fread(dataBuffer_, 1, length, wavFile_);
  if (readsize < length) {
    isEof_ = true;
  }
  readedLength_ += readsize;
  bufDataLength_ = readsize;
}
