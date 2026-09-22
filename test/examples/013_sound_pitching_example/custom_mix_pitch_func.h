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

	// 如果还有声音需要播放
	if (handler->position < handler->duration || handler->loop)
	{
		const float delta = 1000.0f / audio_frequency, // 每个样本的正常持续时间
				vdelta = delta * speed_factor; // 虚拟拉伸持续时间，按“speedFactor”缩放

		// 如果播放不变并且需要音调（第一次）
		if (!handler->altered && speed_factor != 1.0f)
			handler->altered = 1; // 标记播放修改并继续进行音调例程

		if (handler->altered) // 如果没有改变，则跳过此音调例程
		{
			for (int i = 0; i < buffer_size; i += audio_channel_count)
			{
				const int j = i / audio_channel_count;// j 从 0 增加到 size/channelCount，每次增加 1
				const float x = handler->position + j * vdelta; // 获取“虚拟”索引。其对应值将被插入。
				const int k = (int)floor(x / delta); // 获取左索引，从原始块数据中进行插值（右索引将是这个索引加 1）
				const float prop = (x / delta) - k; // 获取正确值的比例（左边将是 1.0 减去这个值）
				// const float prop2 = prop * prop; // 缓存比例的平方（仅三次插值需要）

				// 通常只有 2 个通道：0（左）和 1（右），但谁知道呢……
				for (int c = 0; c < audio_channel_count; c++)
				{
					// check if k will be within bounds
					if (k * audio_channel_count + audio_channel_count - 1 < handler->chunk_size || handler->loop)
					{
						AUDIO_FORMAT_TYPE v0 = chunk_data[(k   * audio_channel_count + c) % handler->chunk_size],
						// v_ = chunk_data[((k-1) * audio_channel_count + c) % handler->chunk_size],
						// v2 = chunk_data[((k+2) * audio_channel_count + c) % handler->chunk_size],
							v1 = chunk_data[((k + 1) * audio_channel_count + c) % handler->chunk_size];

				// 将插值放在“数据”上
				// buffer[i + c] = (1 - prop) * v0 + prop * v1; // 线性插值
				buffer[i + c] = v0 + prop * (v1 - v0); // 线性插值（单乘版本）
				// buffer[i + c] = v0 + 0.5f * prop * ((prop - 3) * v0 - (prop - 2) * 2 * v1 + (prop - 1) * v2); // 二次插值
				// buffer[i + c] = v0 + (prop / 6) * ((3 * prop - prop2 - 2) * v_ + (prop2 - 2 * prop - 1) * 3 * v0 + (prop - prop2 + 2) * 3 * v1 + (prop2 - 1) * v2); // 三次插值
				// buffer[i + c] = v0 + 0.5f * prop * ((2 * prop2 - 3 * prop - 1) * (v0 - v1) + (prop2 - 2 * prop + 1) * (v0 - v_) + (prop2 - prop) * (v2 - v2)); // 三次样条插值
			}
					else  // 如果 k 超出界限（块界限），则意味着我们已经完成；因此，我们将传递沉默
			{
						buffer[i + c] = 0;
			}
		}
	}
}

		// 更新位置
		handler->position += (buffer_size / audio_channel_count) * vdelta;

			// 如果循环则重置位置
		if (handler->loop) while (handler->position > handler->duration)
			handler->position -= handler->duration;
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