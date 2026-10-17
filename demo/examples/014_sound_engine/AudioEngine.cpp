#include "AudioEngine.h"

OG_BEGIN


  Uint16 audio_format; //当前音频格式常量
  int audio_frequency, // 当前音频格式的频率
audio_channel_count, // 当前音频格式的通道数
audio_allocated_mix_channels_count; // 分配的混音通道数量


//Uint8 版本的回调的实现 
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackUint8
#define AUDIO_FORMAT_TYPE Uint8
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

// Sint8 版本回调的实现  
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint8
#define AUDIO_FORMAT_TYPE Sint8
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

//Uint16版本的回调的实现 
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackUint16
#define AUDIO_FORMAT_TYPE Uint16
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

// Sint16 版本的回调的实现  
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint16
#define AUDIO_FORMAT_TYPE Sint16
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

// Sint32 版本的回调的实现  
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint32
#define AUDIO_FORMAT_TYPE Sint32
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

// Float 版本的回调实现  
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackFloat
#define AUDIO_FORMAT_TYPE float
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE


Uint16 format_sample_size(Uint16 format) { 
	return (format & 0xFF) / 8;
}

int Custom_Mix_ComputeChunkLengthMillisec(int chunkSize)
{
	const Uint32 points = chunkSize / format_sample_size(audio_format); // 字节 / samplesize == 样本点
	const Uint32 frames = (points / audio_channel_count); // 样本点/通道 == 样本帧
	return frames;// (int)((frames * 1000) / audio_frequency);  // (sample frames * 1000) / frequency == 播放长度，以毫秒为单位
}

Custom_Mix_PlaybackSpeedEffectHandler* Custom_Mix_CreatePlaybackSpeedEffectHandler(const Mix_Chunk* chunk,
	const float* speed, int loop, int self_halt)
{
	Custom_Mix_PlaybackSpeedEffectHandler* handler = (Custom_Mix_PlaybackSpeedEffectHandler*)malloc(sizeof(Custom_Mix_PlaybackSpeedEffectHandler));
	handler->chunk = chunk;
	handler->speed = speed;
	handler->position = 0;
	handler->altered = 0;
	handler->loop = loop;
	handler->duration = Custom_Mix_ComputeChunkLengthMillisec(chunk->alen);
	handler->chunk_size = chunk->alen / format_sample_size(audio_format);
	handler->self_halt = self_halt;
	return handler;
}


// Mix_EffectDone_t 回调在效果使用结束时删除处理程序（通过 userData 传递的处理程序） 
 void Custom_Mix_PlaybackSpeedEffectDoneCallback(int channel, void *userData)
{
	free(userData);
}
  void Custom_Mix_RegisterPlaybackSpeedEffect(int channel, Mix_Chunk* chunk, float* speed, int loop, int selfHalt)
{
	Mix_EffectFunc_t effect_func_callback;
	// 根据当前音频格式为该频道注册合适的播放速度效果处理程序。效果仅对当前（或下一个）播放有效。 
	switch (audio_format)
	{
	case AUDIO_U8:  effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackUint8;  break;
	case AUDIO_S8:  effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackSint8;  break;
	case AUDIO_U16: effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackUint16; break;
	default:
	case AUDIO_S16: effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackSint16; break;
	case AUDIO_S32: effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackSint32; break;
	case AUDIO_F32: effect_func_callback = Custom_Mix_PlaybackSpeedEffectFuncCallbackFloat;  break;
	}

	Mix_RegisterEffect(channel, effect_func_callback,
		Custom_Mix_PlaybackSpeedEffectDoneCallback, Custom_Mix_CreatePlaybackSpeedEffectHandler(chunk, speed, loop, selfHalt));
}

   

OG_END