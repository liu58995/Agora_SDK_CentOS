//
//  Agora SDK
//
//  Created by Zheng Ender in 2018-01.
//  Copyright (c) 2018 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "IAgoraRtcEngine.h"
#include "api2/IAgoraService.h"
#include "api2/NGIAgoraAudioTrack.h"
#include "api2/NGIAgoraVideoTrack.h"

namespace agora {
namespace media {
class IAudioFrameObserver;
}

namespace rtc {
class IAudioEngineWrapper;

class ILocalUserObserver;
class ILocalVideoTrack;
class IRemoteVideoTrack;

/**
 * The ILocalUser class defines the behavior and state of the local user.
 *
 * Once connected, each RTC connection has its own local user. Apps can get the local
 * user object using \ref agora::rtc::IRtcConnection::getLocalUser
 * "IRtcConnection::getLocalUser".
 *
 * Each local user has two user roles: broadcaster (publisher or subscriber) and
 * audience (subscriber only). The publisher publishes audio and video tracks, while
 * the audience receives them.
 */
class ILocalUser {
 public:
  /**
   * Options of subscribing to video streams.
   */
  struct VideoSubscriptionOptions {
    VideoSubscriptionOptions() : type(rtc::REMOTE_VIDEO_STREAM_HIGH), encodedFrameOnly(false) {}
    /**
     * The type of the video stream to subscribe to: #REMOTE_VIDEO_STREAM_TYPE.
     *
     * The default value is REMOTE_VIDEO_STREAM_HIGH, which means the high-resolution and high-bitrate
     * video stream.
     */
    REMOTE_VIDEO_STREAM_TYPE type;
    /**
     * Determines whether to subscribe to encoded video data only:
     * - true: Subscribe to encoded video data only.
     * - false: (Default) Do not subscribe to encoded video data only.
     */
    bool encodedFrameOnly;
  };

  /**
   * Statistics related to audio network adaptation (ANA).
   */
  struct ANAStats {
    ANAStats() = default;
    ANAStats(const ANAStats&) = default;
    ~ANAStats() = default;
    /**
     * Sets the number of actions taken by the ANA bitrate controller since the start of the call.
     *
     * If you do not set this parameter, the ANA bitrate controller is disabled.
     */
    agora::base::Optional<uint32_t> bitrate_action_counter;
    /**
     * Sets the number of actions taken by the ANA channel controller since the start of the call.
     *
     * If you do not set this parameter, the ANA channel controller is disabled.
     */
    agora::base::Optional<uint32_t> channel_action_counter;
    /**
     * Sets the number of actions taken by the ANA DTX controller since the start of the call.
     *
     * If you do not set this parameter, the ANA DTX controller is disabled.
     */
    agora::base::Optional<uint32_t> dtx_action_counter;
    /**
     * Sets the number of actions taken by the ANA FEC (Forward Error Correction) controller since the start of the call.
     *
     * If you do not set this parameter, the ANA FEC controller is disabled.
     */
    agora::base::Optional<uint32_t> fec_action_counter;
    /**
     * Sets the number of times that the ANA frame length controller decides to increase the frame length
     * since the start of the call.
     *
     * If you do not set this parameter, the ANA frame length controller is disabled.
     */
    agora::base::Optional<uint32_t> frame_length_increase_counter;
    /**
     * Sets the number of times that the ANA frame length controller decides to decrease the frame length
     * since the start of the call.
     *
     * If you so not set this parameter, the ANA frame length controller is disabled.
     */
    agora::base::Optional<uint32_t> frame_length_decrease_counter;
    /**
     * Sets the uplink packet loss fractions set by the ANA FEC controller.
     *
     * If you do not set this parameter, the ANA FEC controller is not active.
     */
    agora::base::Optional<float> uplink_packet_loss_fraction;
  };

  /**
   * Statistics related to audio processing.
   */
  struct AudioProcessingStats {
    AudioProcessingStats() = default;
    AudioProcessingStats(const AudioProcessingStats& other) = default;
    ~AudioProcessingStats() = default;

    /**
     * The echo return loss (ERL).
     *
     * ERL = 10log_10(P_far / P_echo).
     *
     * ERL is a measure of signal loss that comes back as an echo. A higher ratio corresponds to a smaller
     * amount of echo; the higher the ERL the better.
     */
    agora::base::Optional<double> echo_return_loss;
    //
    /**
     * The echo return loss enhancement (ERLE).
     *
     * ERLE = 10log_10(P_echo / P_out).
     *
     * The performance of an echo canceller is measured in echo return loss enhancement, which is the amount
     * of additional signal loss applied by the echo canceller.
     *
     * The total signal loss of the echo is the sum of ERL and ERLE.
     */
    agora::base::Optional<double> echo_return_loss_enhancement;
    /**
     * The fraction of time that the AEC (Acoustic Echo Cancelling) linear filter is divergent, in a
     * 1-second non-overlapped aggregation window.
     */
    agora::base::Optional<double> divergent_filter_fraction;

    /**
     * The delay metrics (ms).
     *
     * It consists of the delay median and standard deviation. It also consists of the
     * fraction of delay estimates that can make the echo cancellation perform poorly. The values are
     * aggregated until the first call of GetStatistics() and afterwards aggregated and updated every
     * second. Note that if there are several clients pulling metrics from
     * GetStatistics() during a session the first call from any of them will change to one second
     * aggregation window for all.
     */
    agora::base::Optional<int32_t> delay_median_ms;
    /**
     * The delay standard deviation(ms).
     */
    agora::base::Optional<int32_t> delay_standard_deviation_ms;

    /**
     * The residual echo detector likelihood.
     */
    agora::base::Optional<double> residual_echo_likelihood;
    /**
     * The maximum residual echo likelihood from the last time period.
     */
    agora::base::Optional<double> residual_echo_likelihood_recent_max;

    /**
     * The instantaneous delay estimate produced in the AEC (ms).
     * The value is the instantaneous value at the time of calling GetStatistics().
     */
    agora::base::Optional<int32_t> delay_ms;
  };

  /**
   * The detailed statistics of the local audio.
   */
  struct LocalAudioDetailedStats {
    LocalAudioDetailedStats() = default;
    ~LocalAudioDetailedStats() = default;

    /**
     * The synchronization source of the local audio.
     */
    uint32_t local_ssrc = 0;
    /**
     * The number of audio bytes sent.
     */
    int64_t bytes_sent = 0;
    /**
     * The number of audio packets sent.
     */
    int32_t packets_sent = 0;
    /**
     * The number of audio packets lost.
     */
    int32_t packets_lost = -1;
    /**
     * The lost fraction.
     */
    float fraction_lost = -1.0f;
    /**
     * The codec name.
     */
    char codec_name[media::kMaxCodecNameLength];
    /**
     * The type of the codec payload.
     */
    agora::base::Optional<int> codec_payload_type;
    /**
     * The ext sequence number.
     */
    int32_t ext_seqnum = -1;
    /**
     * The jitter duration (ms).
     */
    int32_t jitter_ms = -1;
    /**
     * The RTT (Round-Trip Time) duration (ms).
     */
    int64_t rtt_ms = -1;
    /**
     * The audio level.
     */
    int32_t audio_level = -1;
    /**
     * The total input energy.
     */
    double total_input_energy = 0.0;
    /**
     * The total input duration.
     */
    double total_input_duration = 0.0;
    /**
     * Whether the typing noise is detected.
     * - true: The typing noise is detected.
     * - false: The typing noise is not detected.
     */
    bool typing_noise_detected = false;

    ANAStats ana_statistics;
    AudioProcessingStats apm_statistics;
  };

 public:
  virtual ~ILocalUser() {}

  /**
   * Sets the role of the user.
   *
   * You can call this method either before or after connecting to an Agora channel:
   * - Before connecting: This method sets the user role as publisher or subscriber (default).
   * - After connecting: This method allows you to switch the user role between publisher and
   * subscriber.
   *
   * The \ref IRtcConnectionObserver::onChangeRoleSuccess "onChangeRoleSuccess" and
   * \ref IRtcConnectionObserver::onChangeRoleFailure "onChangeRoleFailure"
   * callbacks indicate the result of this method call.
   *
   * @note
   * If the token provided in the \ref IRtcConnection::connect "connect" method does not include the
   * role's privilege, the connection fails with the \ref IRtcConnectionObserver::onConnectionFailure
   * "onConnectionFailure" callback.
   *
   * @param role Role of the user: #CLIENT_ROLE_TYPE.
   */
  virtual void setUserRole(rtc::CLIENT_ROLE_TYPE role) = 0;

  /**
   * Gets the role of the user.
   *
   * @return
   * - The role of the user, if the method call succeeds.
   */
  virtual CLIENT_ROLE_TYPE getUserRole() = 0;

  /**
   * Configures the audio encoder.
   *
   * Once set, the SDK uses the configurations for all the sending audio streams.
   *
   * @param config The reference to the audio encoder configurations: AudioEncoderConfiguration.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setAudioEncoderConfiguration(const rtc::AudioEncoderConfiguration& config) = 0;

  /**
   * Gets the detailed statistics of the local audio.
   *
   * @param stats A reference to the detailed statistics of the local audio: LocalAudioDetailedStats.
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool getLocalAudioStatistics(LocalAudioDetailedStats& stats) = 0;

  /**
   * Publishes a local audio track to the RTC connection.
   *
   * By default, all published audio tracks are mixed.
   *
   * @param audioTrack The local audio track to be published: ILocalAudioTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_REFUSED, if the role of the local user is not broadcaster.
   */
  virtual int publishAudio(agora_refptr<ILocalAudioTrack> audioTrack) = 0;

  /**
   * Stops publishing the local audio track to the RTC connection.
   *
   * @param audioTrack The local audio track that you want to stop publishing: ILocalAudioTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unpublishAudio(agora_refptr<ILocalAudioTrack> audioTrack) = 0;

  /**
   * Publishes a local video track to the RTC connection.
   *
   * @param videoTrack The local video track to be published: ILocalVideoTrack.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   * TODO(Bob)
   */
  virtual int publishVideo(agora_refptr<ILocalVideoTrack> videoTrack) = 0;

  /**
   * Stops publishing the local video track to the RTC connection.
   *
   * @param videoTrack The local video track that you want to stop publishing: ILocalVideoTrack.
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unpublishVideo(agora_refptr<ILocalVideoTrack> videoTrack) = 0;

  /**
   * Subscribes to the audio of a specified remote user in the RTC connection.
   *
   * @param userId ID of the remote user whose audio you want to subscribe to.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT, if no such user exists or the `userId` is invalid.
   */
  virtual int subscribeAudio(user_id_t userId) = 0;

  /**
   * Subscribes to the audio of all remote users in the RTC connection.
   *
   * This method also automatically subscribes to the audio of any subsequent user.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int subscribeAllAudio() = 0;

  /**
   * Stops subscribing to the audio of a specified remote user in the RTC connection.
   *
   * @param userId ID of the user whose audio you want to stop subsribing to.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT, if no such user exists or the `userId` is invalid.
   */
  virtual int unsubscribeAudio(user_id_t userId) = 0;

  /**
   * Stops subscribing to the audio of all remote users in the RTC connection.
   *
   * This method automatically stops subscribing to the audio of any subsequent users, unless you explicitly 
   * call the \ref subscribeAudio "subscribeAudio" or \ref subscribeAllAudio "subscribeAllAudio" method.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unsubscribeAllAudio() = 0;

  /**
   * Adjusts the playback signal volume.
   * @param volume The playback volume. The value ranges between 0 and 100 (default).
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int adjustPlaybackSignalVolume(int volume) = 0;

  /**
   * Gets the current playback signal volume.
   * @param volume A pointer to the playback signal volume.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int getPlaybackSignalVolume(int* volume) = 0;

  /**
   * Pulls mixed PCM audio data from the RTC connection.
   *
   * @note
   * To pull mixed PCM audio data using this method, you must set the `pcmDataOnly` member in AudioSubscriptionOptions to
   * `true` when creating the RTC connection. Otherwise, this method returns false and no valid data is
   * contained in `payload_data`.
   *
   * @param payload_data The pointer to the mixed PCM audio data.
   * @param audioFrameInfo The reference to the information of the PCM audio data: AudioPcmDataInfo.
   * @return
   * - true: Success.
   * - false: Failure.
   */
  virtual bool pullMixedAudioPcmData(void* payload_data, AudioPcmDataInfo &audioFrameInfo) = 0;
  /**
   * Sets the audio frame parameters for the \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame
   * "onPlaybackAudioFrame" callback.
   *
   * @param numberOfChannels The number of channels contained in the onPlaybackAudioFrame callback.
   * - 1: Mono.
   * - 2: Stereo.
   * @param sampleRateHz The sample rate(Hz) contained in the onPlaybackAudioFrame callback. You can
   * set it as 8000, 16000, 32000, 44100, or 48000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz) = 0;
  /**
   * Sets the audio frame parameters for the \ref agora::media::IAudioFrameObserver::onRecordAudioFrame 
   * "onRecordAudioFrame" callback.
   *
   * @param numberOfChannels The number of channels contained in the onRecordAudioFrame callback.
   * - 1: Mono.
   * - 2: Stereo.
   * @param sampleRateHz The sample rate(Hz) contained in the onRecordAudioFrame callback. You can
   * set it as 8000, 16000, 32000, 44100, or 48000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRecordingAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz) = 0;
  /**
   * Sets the audio frame parameters for the \ref agora::media::IAudioFrameObserver::onMixedAudioFrame 
   * "onMixedAudioFrame" callback.
   *
   * @param numberOfChannels The number of channels contained in the onMixedAudioFrame callback.
   * - 1: Mono.
   * - 2: Stereo.
   * @param sampleRateHz The sample rate (Hz) contained in the onMixedAudioFrame callback. You can
   * set it as 8000, 16000, 32000, 44100, or 48000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMixedAudioFrameParameters(size_t numberOfChannels, uint32_t sampleRateHz) = 0;

  /**
   * Sets the audio frame parameters for the \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing 
   * "onPlaybackAudioFrameBeforeMixing" callback.
   *
   * @param numberOfChannels The number of channels contained in the onPlaybackAudioFrameBeforeMixing callback.
   * - 1: Mono.
   * - 2: Stereo.
   * @param sampleRateHz The sample rate(Hz) contained in the onPlaybackAudioFrameBeforeMixing callback. You can
   * set it as 8000, 16000, 32000, 44100, or 48000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setPlaybackAudioFrameBeforeMixingParameters(size_t numberOfChannels,
                                                          uint32_t sampleRateHz) = 0;

  /**
   * Registers an audio frame observer object.
   *
   * You need to implement the IAudioFrameObserver class in this method, and register the following callbacks
   * according to your scenario:
   * - \ref agora::media::IAudioFrameObserver::onRecordAudioFrame "onRecordAudioFrame": Occurs when the SDK receives the audio data captured by the local recording device.
   * - \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame": Occurs when the SDK receives the remote audio data for playback.
   * - \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrameBeforeMixing "onPlaybackAudioFrameBeforeMixing": Occurs when the SDK receives the remote audio data of a specified
   * remote user before mixing.
   * - \ref agora::media::IAudioFrameObserver::onMixedAudioFrame "onMixedAudioFrame": Occurs when the SDK receives the mixed recorded and playback audio data.
   *
   * @param observer A pointer to the audio frame observer: \ref agora::media::IAudioFrameObserver "IAudioFrameObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(agora::media::IAudioFrameObserver * observer) = 0;
  /**
   * Releases the audio frame observer object.
   *
   * @param observer The pointer to the audio frame observer: \ref agora::media::IAudioFrameObserver "IAudioFrameObserver".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterAudioFrameObserver(agora::media::IAudioFrameObserver * observer) = 0;

  /**
   * Subscribes to the video of a specified remote user in the RTC connection.
   *
   * @param userId ID of the user whose video you want to subscribe to.
   * @param subscriptionOptions The reference to the video subscription options. i.e., subscribe encoded video data only and/or
   * subscribe REMOTE_VIDEO_STREAM_LOW with lower bitrate.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT, if the `userId` is invalid.
   */
  virtual int subscribeVideo(user_id_t userId,
                             const VideoSubscriptionOptions &subscriptionOptions) = 0;

  /**
   * Subscribes to the video of all remote users in the RTC connection.
   *
   * This method also automatically subscribes to the video of any subsequent remote user.
   *
   * @param subscriptionOptions The reference to the video subscription options: VideoSubscriptionOptions. You can decide whether
   * you want to subscribe to encoded video data only.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int subscribeAllVideo(const VideoSubscriptionOptions &subscriptionOptions) = 0;

  /**
   * Stops subscribing to the video of a specified remote user in the RTC connection.
   *
   * @param userId ID of the user whose video stream you want to stop subscribing to.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT, if the `userId` is invalid.
   */
  virtual int unsubscribeVideo(user_id_t userId) = 0;

  /**
   * Stops subscribing to the video of all remote users in the RTC connection.
   *
   * This method automatically stops subscribing to the video of any subsequent user, unless you explicitly
   * call \ref subscribeVideo "subscribeVideo" or \ref subscribeAllVideo "subscribeAllVideo".
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unsubscribeAllVideo() = 0;

  /** 
   * Sets the time interval and the volume smoothing factor of the onAudioVolumeIndication callback.
   *
   * By default, the SDK triggers the \ref ILocalUserObserver::onAudioVolumeIndication "onAudioVolumeIndication" 
   * callback once every 500 ms, with a smoothing factor of 3.
   *
   * @param intervalInMS Sets the time interval(ms) between two consecutive volume indications. The default
   * value is 500.
   * - &le; 10: Disables the volume indication.
   * - > 10: The time interval (ms) between two consecutive callbacks. 
   *
   * @param smooth The smoothing factor that sets the sensitivity of the audio volume indicator.
   * The value range is [0, 10]. The greater the value, the more sensitive the indicator.The default value is 3.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT, if the `intervalInMS` or `smooth` is set out of range.
   */
  virtual int setAudioVolumeIndicationParameters(int intervalInMS, int smooth) = 0;

  /**
   * Registers a local user observer object.
   *
   * You need to implement the ILocalUserObserver class in this method. Once registered, the
   * ILocalUserObserver receives events sent by the ILocalUser object.
   *
   * @param observer The pointer to the ILocalUserObserver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerLocalUserObserver(ILocalUserObserver* observer) = 0;

  /**
   * Releases the ILocalUserObserver object previously registered using registerLocalUserObserver().
   *
   * @param observer The pointer to the ILocalUserObserver object that you want to release.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterLocalUserObserver(ILocalUserObserver* observer) = 0;

  /**
   * Get media control packet sender.
   *
   * @return
   * - The pointer to the media control packet sender, if the method call succeeds.
   * - NULL, if the method call fails.
   */
  virtual IMediaControlPacketSender* getMediaControlPacketSender() = 0;

  /**
   * Registers an IMediaControlPacketReceiver object.
   *
   * You need to implement the IMediaControlPacketReceiver class in this method. Once you successfully
   * register the media control packet receiver, the SDK triggers the onMediaControlPacketReceived
   * callback when it receives the media control packet.
   *
   * @param ctrlPacketReceiver The pointer to the IMediaControlPacketReceiver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int registerMediaControlPacketReceiver(IMediaControlPacketReceiver* ctrlPacketReceiver) = 0;

  /**
   * Releases the IMediaControlPacketReceiver object.
   * @param ctrlPacketReceiver The pointer to the IMediaControlPacketReceiver object.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unregisterMediaControlPacketReceiver(IMediaControlPacketReceiver* ctrlPacketReceiver) = 0;

  /**
   * Send intra request to the broadcaster with specific UID.
   * @param uid The target broadcaster uid.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendIntraRequest(uid_t uid) = 0;
};

/**
 * The ILocalUserObserver class.
 */
class ILocalUserObserver {
 public:
  virtual ~ILocalUserObserver() {}

  /**
   * Occurs when the first packet of the local audio track is sent, indicating that the local audio track
   * is successfully published.
   *
   * @param audioTrack The pointer to ILocalAudioTrack.
   */
  virtual void onAudioTrackPublishSuccess(agora_refptr<ILocalAudioTrack> audioTrack) = 0;

  /**
   * Occurs when the local audio track fails to be published.
   *
   * @param audioTrack The pointer to ILocalAudioTrack.
   * @param error The error information: #ERROR_CODE_TYPE.
   */
  virtual void onAudioTrackPublicationFailure(agora_refptr<ILocalAudioTrack> audioTrack,
                                              ERROR_CODE_TYPE error) = 0;

  /**
   * Occurs when the state of the local audio track changes.
   *
   * @param audioTrack The pointer to ILocalAudioTrack.
   * @param state The state of the local audio track: #LOCAL_AUDIO_STREAM_STATE.
   * @param errorCode The error information for a state failure: #LOCAL_AUDIO_STREAM_ERROR.
   */
  virtual void onLocalAudioTrackStateChanged(agora_refptr<rtc::ILocalAudioTrack> audioTrack,
                                             LOCAL_AUDIO_STREAM_STATE state,
                                             LOCAL_AUDIO_STREAM_ERROR errorCode) = 0;
  /**
   * Reports the statistics of the local audio track.
   *
   * @param stats The statistics of the local audio track: LocalAudioTrackStats.
   */
  virtual void onLocalAudioTrackStatistics(const LocalAudioStats& stats) = 0;
  /**
   * Reports the statistics of the remote audio track.
   *
   * @param audioTrack The pointer to IRemoteAudioTrack.
   * @param stats The statistics of the remote audio track: RemoteAudioTrackStats.
   */
  virtual void onRemoteAudioTrackStatistics(agora_refptr<rtc::IRemoteAudioTrack> audioTrack, const RemoteAudioTrackStats& stats) = 0;
  /**
   * Occurs when the first remote audio frame is received.
   *
   * This callback indicates that the local user has subscribed to a specified remote audio track, and the first
   * frame of this audio track has been received.
   *
   * @param userId ID of the remote user whose audio frame is received.
   * @param audioTrack The pointer to IRemoteAudioTrack.
   */
  virtual void onUserAudioTrackSubscribed(user_id_t userId,
                                          agora_refptr<rtc::IRemoteAudioTrack> audioTrack) = 0;

  /**
   * Occurs when the state of a remote audio track changes.
   *
   * @param userId ID of the remote user whose audio track state has changed.
   * @param audioTrack The pointer to IRemoteAudioTrack.
   * @param state The current state of the audio track: #REMOTE_AUDIO_STATE.
   * @param reason The reason for the state change: #REMOTE_AUDIO_STATE_REASON.
   */
  virtual void onUserAudioTrackStateChanged(user_id_t userId,
                                            agora_refptr<rtc::IRemoteAudioTrack> audioTrack,
                                            REMOTE_AUDIO_STATE state,
                                            REMOTE_AUDIO_STATE_REASON reason,
                                            int elapsed) = 0;

  /**
   * Occurs when the first packet of the local video track is sent, indicating that the local video track
   * is successfully published.
   * @param videoTrack The pointer to ILocalVideoTrack.
   */
  virtual void onVideoTrackPublishSuccess(agora_refptr<ILocalVideoTrack> videoTrack, int elapsed) = 0;

  /**
   * Occurs when the local video track fails to be published.
   *
   * @param videoTrack The pointer to ILocalVideoTrack.
   * @param error The error information: #ERROR_CODE_TYPE.
   */
  virtual void onVideoTrackPublicationFailure(agora_refptr<ILocalVideoTrack> videoTrack,
                                              ERROR_CODE_TYPE error) = 0;

  /**
   * Occurs when the state of the local video track changes.
   *
   * @param videoTrack The pointer to ILocalVideoTrack.
   * @param state The state of the local video track: #LOCAL_VIDEO_STREAM_STATE.
   * @param errorCode The error information for a state failure: #LOCAL_VIDEO_STREAM_ERROR.
   */
  virtual void onLocalVideoTrackStateChanged(agora_refptr<rtc::ILocalVideoTrack> videoTrack,
                                             LOCAL_VIDEO_STREAM_STATE state,
                                             LOCAL_VIDEO_STREAM_ERROR errorCode) = 0;

  /**
   * Reports the statistics of the local video track.
   *
   * @param videoTrack The pointer to ILocalVideoTrack.
   * @param stats The statistics of the local video track: LocalVideoTrackStats.
   */
  virtual void onLocalVideoTrackStatistics(agora_refptr<rtc::ILocalVideoTrack> videoTrack,
                                           const LocalVideoTrackStats& stats) = 0;

  /**
   * Occurs when the first remote video frame is received.
   *
   * This callback indicates that the local user has subscribed to a specified remote video track, and 
   * the first frame of this video track has been received.
   *
   * @param userId ID of the remote user whose video frame is received.
   * @param trackInfo The information of the remote video track: RemoteVideoTrackInfo.
   * @param videoTrack The pointer to IRemoteVideoTrack.
   */
  virtual void onUserVideoTrackSubscribed(user_id_t userId, VideoTrackInfo trackInfo,
                                          agora_refptr<rtc::IRemoteVideoTrack> videoTrack) = 0;

  /**
   * Occurs when the state of a remote video track changes.
   *
   * @param userId ID of the remote user whose video track state has changed.
   * @param videoTrack The pointer to the IRemoteVideoTrack object.
   * @param state The current state of the video track: #REMOTE_VIDEO_STREAM_STATE.
   * @param reason The reason for the state change: #REMOTE_VIDEO_STATE_REASON.
   */
  virtual void onUserVideoTrackStateChanged(user_id_t userId,
                                            agora_refptr<rtc::IRemoteVideoTrack> videoTrack,
                                            REMOTE_VIDEO_STREAM_STATE state,
                                            REMOTE_VIDEO_STATE_REASON reason,
                                            int elapsed) = 0;

  /**
   * Reports the statistics of the remote video track.
   *
   * @param videoTrack The pointer to IRemoteVideoTrack.
   * @param stats The statistics of the remote video track: RemoteVideoTrackStats.
   */
  virtual void onRemoteVideoTrackStatistics(agora_refptr<rtc::IRemoteVideoTrack> videoTrack,
                                            const RemoteVideoTrackStats& stats) = 0;


  /**
   * Reports which users are speaking, the speakers' volume, and whether the local user is speaking.
   *
   * This callback reports the IDs and volumes of the loudest speakers at the moment in the channel, 
   * and whether the local user is speaking.
   *
   * You can set time interval of this callback using \ref ILocalUser::setAudioVolumeIndicationParameters "setAudioVolumeIndicationParameters".
   *
   * The SDK triggers two independent `onAudioVolumeIndication` callbacks at one time, which seperately report the
   * volume information of the local user and all the remote users.
   *
   * @note
   * - To detect whether the local user is speaking, set `report_vad` in `enableAudioVolumeIndication` to true.
   * - Calling the `muteLocalAudioStream` method affects the SDK's behavior in the following ways:
   *   - If the local user calls the `muteLocalAudioStream` method, the SDK stops triggering the `onAudioVolumeIndication` callback corresponding to the local user.
   *   - 20 seconds after a remote speaker calls `muteLocalAudioStream`, the `onAudioVolumeIndication` callback corresponding to the remote
   * speakers does not include information on this remote user; 20 seconds after all remote users call `muteLocalAudioStream`, the SDK stops
   * triggering this callback for the remote speakers.
   *
   * @param speakers The pointer to AudioVolumeInfo, which is an array containing the user ID and volume information for each speaker: AudioVolumeInfo.
   * - In the local user's callback, this array contains the following members:
   *   - `uid` = 0,
   *   - `volume` = `totalVolume`, which reports the sum of the voice volume and audio-mixing volume of the local user, and
   *   - `vad`, which reports whether the local user is speaking.
   * - In the remote users' callback, this array contains the following members:
   *   - `uid` of each remote speaker.
   *   - `volume`, which reports the sum of the voice volume and audio-mixing volume of each remote speaker.
   *   - `vad` = 0.
   * An empty `speakers` array in the callback indicates that no remote user is speaking at the moment.
   * @param speakerNumber Total number of the speakers.
   * @param totalVolume Total volume after audio mixing. The value ranges between 0 (lowest volume) and 255 (highest volume).
   * - In the local user's callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of the local user.
   * - In the remote speakers' callback, `totalVolume` is the sum of the voice volume and audio-mixing volume of all remote speakers.
   */
  virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber,
                                       int totalVolume) = 0;

  /**
   * Media information of a specified user.
   */
  enum USER_MEDIA_INFO {
    /**
     * 0: The user has muted audio.
     */
    USER_MEDIA_INFO_MUTE_AUDIO = 0,
    /**
     * 1: The user has muted video.
     */
    USER_MEDIA_INFO_MUTE_VIDEO = 1,
    /**
     * 4: The user has enabled video, which includes video capturing and encoding.
     */
    USER_MEDIA_INFO_ENABLE_VIDEO = 4,
    /**
     * 8: The user has enabled the local video capturing.
     */
    USER_MEDIA_INFO_ENABLE_LOCAL_VIDEO = 8,
  };

  /**
   * Occurs when the user information is updated.
   *
   * The function is for handling the network message from PeerManager::updatePeerState.
   *
   * @param userId ID of the user.
   * @param msg The media information of the user: #USER_MEDIA_INFO.
   * @param val Whether
   */
  virtual void onUserInfoUpdated(user_id_t userId, USER_MEDIA_INFO msg, bool val) {
    (void)userId;
    (void)msg;
    (void)val;
  }

  /**
   * Occurs when the intra request is received from remote user.
   *
   * The function is for notifying local user to encode a Key frame.
   *
   */
  virtual void onIntraRequestReceived() {}
};

}  // namespace rtc
}  // namespace agora
