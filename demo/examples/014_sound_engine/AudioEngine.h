#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#pragma

#include "SDLmanager.h"
#include "OGPlatformMacros.h"
#include "OGHeader.h" 

#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <algorithm>

OG_BEGIN

// 自定义处理程序对象来控制播放 Mix_Chunk 的音频数据的哪一部分，以及进行哪些与音高相关的修改。
typedef struct Custom_Mix_PlaybackSpeedEffectHandler
{
	const Mix_Chunk* chunk;
	const float* speed;
	float position;
	int altered;

	int loop;
	int duration;
	int chunk_size;
	int self_halt;
} Custom_Mix_PlaybackSpeedEffectHandler;

extern Uint16 audio_format;
extern int audio_frequency,
audio_channel_count,
audio_allocated_mix_channels_count;
extern void Custom_Mix_PlaybackSpeedEffectFuncCallback(int mix_channel, void* stream, int length, void* user_data);
extern Uint16 format_sample_size(Uint16 format);
extern int Custom_Mix_ComputeChunkLengthMillisec(int chunkSize);
extern Custom_Mix_PlaybackSpeedEffectHandler* Custom_Mix_CreatePlaybackSpeedEffectHandler(const Mix_Chunk* chunk,
	const float* speed, int loop, int self_halt);
extern void Custom_Mix_PlaybackSpeedEffectDoneCallback(int channel, void *userData);
extern void Custom_Mix_RegisterPlaybackSpeedEffect(int channel, Mix_Chunk* chunk, float* speed, int loop, int selfHalt);

// 每首音频的播放实例（按 ID 管理）
struct MyTrack {
	std::vector<int> channels;   // 这首音频当前占用的 channel（可能多个实例）
	Mix_Chunk* wav = nullptr;    // 指向 wav_cache 里的 chunk（不拥有）
	std::string path;            // 记录路径，方便调试
};

class AudioEngine {
	std::mutex _play2dMutex;

	float speed = 1.0f;   // ★ 全局变速（唯一）

	// 路径 → Mix_Chunk 缓存（按路径共享，只加载一次）
	std::unordered_map<std::string, Mix_Chunk*> wav_cache;

	// audioID → 播放实例（每次 play2d 新建）
	std::unordered_map<int, MyTrack*> tracks;
	int _nextAudioID = 0;

	// 内部：加载或复用 chunk
	Mix_Chunk* getOrLoadChunk(const std::string& wav_path) {
		auto it = wav_cache.find(wav_path);
		if (it != wav_cache.end()) {
			return it->second;
		}

		Mix_Chunk* chunk = Mix_LoadWAV(wav_path.c_str());
		if (!chunk) {
			return nullptr;
		}
		wav_cache[wav_path] = chunk;
		return chunk;
	}

public:
	static AudioEngine* getInstace() {
		static AudioEngine _instance;
		return &_instance;
	}

	AudioEngine() {
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
		Mix_QuerySpec(&audio_frequency, &audio_format, &audio_channel_count);
		audio_allocated_mix_channels_count = Mix_AllocateChannels(MIX_CHANNELS);
	}

	~AudioEngine() {
		free();
		Mix_CloseAudio();
		Mix_Quit();
	}

	// ★ 释放所有 chunk 和实例
	void free() {
		// 停掉所有 channel
		for (auto& pair : tracks) {
			for (auto& ch : pair.second->channels) {
				Mix_HaltChannel(ch);
			}
			delete pair.second;
		}
		tracks.clear();

		// 释放所有 chunk
		for (auto& pair : wav_cache) {
			Mix_FreeChunk(pair.second);
		}
		wav_cache.clear();
	}

	// ★ play2d 返回 audioID，不再返回 bool
	static int play2d(const std::string& wav_path) {
		auto audio = AudioEngine::getInstace();
		if (wav_path.empty()) return -1;
		return audio->_play2d(wav_path);
	}

	// 内部：加载 + 播放，返回 audioID
	int _play2d(const std::string& wav_path) {
		std::lock_guard<std::mutex> lk(_play2dMutex);

		Mix_Chunk* chunk = getOrLoadChunk(wav_path);
		if (!chunk) return -1;

		int channel = Mix_PlayChannelTimed(-1, chunk, 0, -1);
		if (channel == -1) return -1;

		int audioID = _nextAudioID++;
		MyTrack* track = new MyTrack;
		track->wav = chunk;
		track->path = wav_path;
		track->channels.push_back(channel);

		// ★ 注册 effect，传全局 speed 的地址
		Custom_Mix_RegisterPlaybackSpeedEffect(channel, chunk, &speed, 0, 0);

		tracks[audioID] = track;
		return audioID;
	}

	// ★ 按 ID 暂停
	static void pause(int audioID) {
		auto audio = AudioEngine::getInstace();
		auto it = audio->tracks.find(audioID);
		if (it == audio->tracks.end()) return;
		for (auto& ch : it->second->channels) {
			Mix_Pause(ch);
		}
	}

	// ★ 按 ID 恢复
	static void resume(int audioID) {
		auto audio = AudioEngine::getInstace();
		auto it = audio->tracks.find(audioID);
		if (it == audio->tracks.end()) return;
		for (auto& ch : it->second->channels) {
			Mix_Resume(ch);
		}
	}

	// ★ 按 ID 停止并释放实例
	static void stop(int audioID) {
		auto audio = AudioEngine::getInstace();
		auto it = audio->tracks.find(audioID);
		if (it == audio->tracks.end()) return;

		for (auto& ch : it->second->channels) {
			Mix_HaltChannel(ch);
		}
		delete it->second;
		audio->tracks.erase(it);
	}

	// 全局暂停
	static void pauseAll() {
		auto audio = AudioEngine::getInstace();
		for (auto& pair : audio->tracks) {
			for (auto& ch : pair.second->channels) {
				Mix_Pause(ch);
			}
		}
	}

	// 全局恢复
	static void resumeAll() {
		auto audio = AudioEngine::getInstace();
		for (auto& pair : audio->tracks) {
			for (auto& ch : pair.second->channels) {
				Mix_Resume(ch);
			}
		}
	}

	// 全局停止（停止 + 释放所有实例，但保留 chunk 缓存）
	static void stopAll() {
		auto audio = AudioEngine::getInstace();
		for (auto& pair : audio->tracks) {
			for (auto& ch : pair.second->channels) {
				Mix_HaltChannel(ch);
			}
			delete pair.second;
		}
		audio->tracks.clear();
	}

	// 清空所有缓存（chunk + 实例）
	static void closeAll() {
		auto audio = AudioEngine::getInstace();
		audio->free();
	}
	//每帧调用
	static void cleanupFinished() {
		auto audio = AudioEngine::getInstace();
		std::lock_guard<std::mutex> lk(audio->_play2dMutex);

		for (auto it = audio->tracks.begin(); it != audio->tracks.end(); ) {
			MyTrack* track = it->second;
			bool allStopped = true;
			for (int ch : track->channels) {
				if (ch != -1 && Mix_Playing(ch)) {
					allStopped = false;
					break;
				}
			}
			if (allStopped) {
				for (int ch : track->channels) {
					if (ch != -1) Mix_UnregisterAllEffects(ch);
				}
				delete track;
				it = audio->tracks.erase(it);
			}
			else {
				++it;
			}
		}
	}
	// ★ 全局变速
	void speedInc() {
		speed += 0.1f;
		if (speed > 3.0f) speed = 3.0f;
	}
	void speedDec() {
		speed -= 0.1f;
		if (speed < 0.3f) speed = 0.3f;
	}
	float getSpeed() const { return speed; }
};

OG_END

#endif