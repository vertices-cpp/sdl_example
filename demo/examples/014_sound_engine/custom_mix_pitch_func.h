 

/*
 *自定义混合音高函数.h
 *
 * 创建于：2018 年 1 月 10 日
 * 作者：carlosfaruolo
 */

 // Mix_EffectFunc_t 回调重定向到处理程序方法（处理程序通过 user_data 传递）
 // 处理函数能够改变块速度/音调。
 // AUDIO_FORMAT_TYPE 取决于当前音频格式（可通过 Mix_QuerySpec 查询）
  void Custom_Mix_PlaybackSpeedEffectFuncCallback(int mix_channel, void* stream, int length, void* user_data)
{
	 

	Custom_Mix_PlaybackSpeedEffectHandler* handler = (Custom_Mix_PlaybackSpeedEffectHandler*)user_data;
	const AUDIO_FORMAT_TYPE* chunk_data = (AUDIO_FORMAT_TYPE*)handler->chunk->abuf;

	AUDIO_FORMAT_TYPE* buffer = (AUDIO_FORMAT_TYPE*)stream;
	const int buffer_size = length / sizeof(AUDIO_FORMAT_TYPE);  //缓冲区大小（作为数组）
	const float speed_factor = *(handler->speed); // 对速度因子进行“快照”

// 	printf("altered=%d, position=%.2f, duration=%d, speed=%.4f\n",
// 		handler->altered, handler->position, handler->duration, speed_factor);

	// 如果还有声音需要播放
	if (handler->position < handler->duration || handler->loop)
	{
		const float delta = 1.0f,// 1000.0f / audio_frequency, // 每个样本的正常持续时间
			vdelta = delta * speed_factor; // 虚拟拉伸持续时间，按“speedFactor”缩放

	// 如果播放不变并且需要音调（第一次）
		if (!handler->altered && speed_factor != 1.0f)
			handler->altered = 1; // 标记播放修改并继续进行音调例程

		if (handler->altered)
		{
			const int buffer_frames = buffer_size / audio_channel_count;

			// 循环时先把 position 规整到 [0, duration)
			float base_position = handler->position;
			if (handler->loop && handler->duration > 0) {
				base_position = fmod(base_position, (float)handler->duration);
				if (base_position < 0) base_position += handler->duration;
			}

			for (int i = 0; i < buffer_frames; i++)
			{
				const float x = base_position + i * vdelta;
				int k = (int)floor(x / delta);
				const float prop = (x / delta) - k;

				// 循环时取模到 [0, duration)
				if (handler->loop && handler->duration > 0) {
					k = k % handler->duration;
					if (k < 0) k += handler->duration;
				}

				for (int c = 0; c < audio_channel_count; c++)
				{
					if (k * audio_channel_count + audio_channel_count - 1 < handler->chunk_size)
					{
						int idx0 = k * audio_channel_count + c;
						int idx1 = (k + 1) * audio_channel_count + c;
						if (idx1 >= handler->chunk_size) idx1 = c;   // 回绕到开头

						AUDIO_FORMAT_TYPE v0 = chunk_data[idx0];
						AUDIO_FORMAT_TYPE v1 = chunk_data[idx1];

						float sample = (float)v0 + prop * (float)(v1 - v0);
						if (sample > 32767.0f) sample = 32767.0f;
						if (sample < -32768.0f) sample = -32768.0f;
						buffer[i * audio_channel_count + c] = (AUDIO_FORMAT_TYPE)sample;
					}
					else
					{
						buffer[i * audio_channel_count + c] = 0;
					}
				}
			}
		}

		// 更新位置
		handler->position += (buffer_size / audio_channel_count) * vdelta;

		if (handler->loop && handler->duration > 0) {
			handler->position = fmod(handler->position, (float)handler->duration);
			if (handler->position < 0) handler->position += handler->duration;
		}
	}
	else // 如果我们已经播放了整个声音但比 SDL_mixer 预期的更早完成（由于播放速度更快）
	{
		// 设置缓冲区的静音，因为 Mix_HaltChannel() 会在几毫秒内将其中的一部分排出。
		for (int i = 0; i < buffer_size; i++)
			buffer[i] = 0;

		if (handler->self_halt)
			Mix_HaltChannel(mix_channel);  // XXX 不安全调用，因为它锁定了音频；但尚未找到更安全的解决方案……
	}
}
 