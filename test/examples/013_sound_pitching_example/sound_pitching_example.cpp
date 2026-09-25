/*
 *声音_音调_示例.c
 *
 * 创建于：2018 年 1 月 10 日
 * 作者：carlosfaruolo
 */

#if defined(_WIN32)||defined(_WIN64)

#include <SDL.h>
#include <SDL_mixer.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "path_head.h"

/* 全局变量 */
Uint16 audio_format; //当前音频格式常量
int audio_frequency, // 当前音频格式的频率
audio_channel_count, // 当前音频格式的通道数
audio_allocated_mix_channels_count; // 分配的混音通道数量

static Uint16 format_sample_size(Uint16 format) { return (format & 0xFF) / 8; }

/* 给定其大小和当前音频格式，获取块时间长度（以毫秒为单位） */
int Custom_Mix_ComputeChunkLengthMillisec(int chunkSize)
{
	const Uint32 points = chunkSize / format_sample_size(audio_format); // 字节 / samplesize == 样本点
	const Uint32 frames = (points / audio_channel_count); // 样本点/通道 == 样本帧
	return frames;// ((frames * 1000) / audio_frequency);  // (sample frames * 1000) / frequency == 播放长度，以毫秒为单位
}

/* 自定义处理程序对象来控制播放 Mix_Chunk 的音频数据的哪一部分，以及进行哪些与音高相关的修改。 */
typedef struct Custom_Mix_PlaybackSpeedEffectHandler
{
	const Mix_Chunk* chunk;
	const float* speed;  /* ptr 指向所需的播放速度 */
	float position; /* 声音的当前位置，以毫秒为单位 */
		int altered; /* 如果此播放从未被调音，则为 false。*/

		// 只读！
	int loop; /*这是否是循环播放*/
	int duration; /* 声音的持续时间，以毫秒为单位 */
	int chunk_size; /* 声音的大小，以索引数（或样本点）表示。当使用适当的数组类型（而不仅仅是 Uint8*）时，将其视为数组大小。*/
	int self_halt; /* 标记播放完成时是否应通过此回调停止播放 */
} Custom_Mix_PlaybackSpeedEffectHandler;

/* Custom_Mix_PlaybackSpeedEffectHandler 的“构造函数”*/
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

/*Uint8 版本的回调的实现*/
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackUint8
#define AUDIO_FORMAT_TYPE Uint8
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/* Sint8 版本回调的实现 */
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint8
#define AUDIO_FORMAT_TYPE Sint8
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/*Uint16版本的回调的实现*/
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackUint16
#define AUDIO_FORMAT_TYPE Uint16
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/* Sint16 版本的回调的实现 */
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint16
#define AUDIO_FORMAT_TYPE Sint16
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/* Sint32 版本的回调的实现 */
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackSint32
#define AUDIO_FORMAT_TYPE Sint32
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/* Float 版本的回调实现 */
#define Custom_Mix_PlaybackSpeedEffectFuncCallback Custom_Mix_PlaybackSpeedEffectFuncCallbackFloat
#define AUDIO_FORMAT_TYPE float
#include "custom_mix_pitch_func.h"
#undef Custom_Mix_PlaybackSpeedEffectFuncCallback
#undef AUDIO_FORMAT_TYPE

/* Mix_EffectDone_t 回调在效果使用结束时删除处理程序（通过 userData 传递的处理程序）*/
void Custom_Mix_PlaybackSpeedEffectDoneCallback(int channel, void *userData)
{
	free(userData);
}

/* 根据当前音频格式为该频道注册合适的播放速度效果处理程序。效果仅对当前（或下一个）播放有效。*/
void Custom_Mix_RegisterPlaybackSpeedEffect(int channel, Mix_Chunk* chunk, float* speed, int loop, int selfHalt)
{
	Mix_EffectFunc_t effect_func_callback;

		/* 选择当前音频格式的注册函数并使用兼容的处理程序注册效果
		   xxx 对所有 S16 和 U16 格式采取相同的行为是否正确？我们是否应该分别为 AUDIO_S16SYS、AUDIO_S16LSB、AUDIO_S16MSB 等创建 case 语句？*/
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


/* 例子
   运行可执行文件并传递 SDL_mixer 能够打开的声音文件的文件名 (ogg、wav 等) */
int main(int argc, char** argv)
{
	//if (argc < 2) { puts("Missing argument."); return 0; }

	SDL_Init(SDL_INIT_AUDIO);
	SDL_Window *win = SDL_CreateWindow(" ", 0, 0, 640, 480, 0);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
	Mix_QuerySpec(&audio_frequency, &audio_format, &audio_channel_count);  /* 查询规格 */
	//创建一个音频通道，可以使用Mix_AllocateChannels()函数设置音频通道的数量
	audio_allocated_mix_channels_count = Mix_AllocateChannels(MIX_CHANNELS);

	float speed = 1.0;
	Mix_Chunk* chunk = Mix_LoadWAV(og::checkPath("013_sound_pitching_example/0013.wav").c_str());

	if (chunk != NULL)
	{
		//混音播放频道
		const int channel = Mix_PlayChannelTimed(-1, chunk, 0, -1);
		Custom_Mix_RegisterPlaybackSpeedEffect(channel, chunk, &speed, 1, 0);

		puts("Looping for 8 seconds, changing the pitch dynamically...\n");

		/* 循环 8 秒，动态改变音调 */
// 		while (SDL_GetTicks() < 8000);
// 			speed = 1 + 0.25*sin(0.001*SDL_GetTicks());
		SDL_Event e;
		bool quit = false;
		while(!quit)
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_0:
					speed += 1;
					break;
				case SDLK_1:
					speed -= 1;
					break;
				case SDLK_2:
					Mix_HaltMusic();
					break;
				case SDLK_3:
					Mix_Resume(channel);
					break;
				default:
					break;
				}
			}
		}
		puts("Finished.");
	}
	else
		puts("No data.");

	Mix_FreeChunk(chunk);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}