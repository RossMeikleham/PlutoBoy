#ifndef _AUDIO_H
#define _AUDIO_H

//#include "nhes.h"
#include "Basic_Gb_Apu.h"
#include "Sound_Queue.h"

#include "SDL/SDL.h"
#include <stdint.h>


// Blargg's Sound APU Emulator
//
// Simple Wrapper by ShizZy

struct AUDIO
{
	Basic_Gb_Apu apu;
	Sound_Queue sound;

	void Initialize()
	{
		long const sample_rate = 44100;

		if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
		{
			atexit( SDL_Quit );
		}

		apu.set_sample_rate(sample_rate);
		sound.start(sample_rate, 1);
	}

	void WriteRegister(uint16_t address, uint8_t data)
	{
		apu.write_register(address, data);
	}

	unsigned char ReadRegister(uint16_t address)
	{
		uint8_t data;
		data=apu.read_register(address);
		return data;
	}

	void EndFrame()
	{
		apu.end_frame();
			
		// Samples from the frame can now be read out of the apu, or
		// allowed to accumulate and read out later. Use samples_avail()
		// to find out how many samples are currently in the buffer.
			
		int const buf_size = 2048;
		static blip_sample_t buf [buf_size];
			
		// Play whatever samples are available
		long count = apu.read_samples( buf, buf_size );
		sound.write( buf, count );
	}
};

#endif // _AUDIO_
