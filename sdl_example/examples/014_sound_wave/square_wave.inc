     //计算每秒的样本字节 //最大缓冲区
      gBufferByteMaxPosition = samplesFreq * bytesPerSample;
    
      //分配内存
      musicData = new short[gBufferByteMaxPosition];
    
      int typeLength = sizeof(*musicData);
      memset(musicData, 0, gBufferByteMaxPosition * typeLength);
    
      int curLen = gBufferByteMaxPosition / sizeof(*musicData);
     float mVolume = 0;
      int i = 0;
      int startPoint = 0;
      for (; i < curLen; i++) //Fill array with frequencies, mathy-math stuff
      {
    	  if (mVolume < audioVolume)
    		  mVolume += audioVolume * 0.001;
    	  else
    	  {
    		  mVolume = audioVolume;
    	  }
    	  musicData[i] = mVolume * sin(cycle);
    
    	  cycle += curFreq * PI2 / samplesFreq;
    	  if (cycle >= PI2)
    		  cycle -= PI2;
    
      }
      int  cnt = 0; startPoint = 0;
    
      for (; i < gBufferByteMaxPosition; i++) //Fill array with frequencies, mathy-math stuff
      {
    
    	  if (mVolume > 0)
    		  mVolume *= 0.99f;
    	  else
    	  {
    		  gBufferByteMaxPosition = i;
    		  break;
    
    	  }
    
    	  musicData[i] = mVolume * sin(cycle);
    
    	  cycle += curFreq * PI2 / samplesFreq;
    	  if (cycle >= PI2)
    		  cycle -= PI2;
    
      }
      gBufferBytePosition = 0;
    
      SDL_PauseAudioDevice(id, 0);