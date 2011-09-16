/*
 *   Author: Damien Di Fede <ddf@compartmental.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "libsndAudioRecordingStream.h"
#include "AudioSystem.h"

libsndAudioRecordingStream::libsndAudioRecordingStream( const char * filePath, const int bufferSize )
: m_filePath( filePath )
, m_bufferSize( bufferSize )
, m_sndFile(NULL)
, m_readBuffer(NULL)
, m_readBufferLength(0)
, m_bPlaying(false)
{

}

libsndAudioRecordingStream::~libsndAudioRecordingStream()
{
	// make sure we are closed, this releases all resources
	close();
}

void libsndAudioRecordingStream::open()
{
	if ( !m_sndFile )
	{
		m_fileInfo.format = 0;
		m_sndFile = sf_open( m_filePath.c_str(), SFM_READ, &m_fileInfo );

		// now we can make our read buffer
		m_readBufferLength = m_bufferSize * m_fileInfo.channels;
		m_readBuffer = new float[ m_readBufferLength ];
		memset( m_readBuffer, 0, m_readBufferLength*sizeof(float) );
	}
}

void libsndAudioRecordingStream::close()
{
	if ( m_sndFile )
	{
		sf_close( m_sndFile );
		m_sndFile = NULL;

		delete [] m_readBuffer;
		m_readBuffer = NULL;
		m_readBufferLength = 0;
	}
}

void libsndAudioRecordingStream::play()
{
	m_bPlaying = true;
}

void libsndAudioRecordingStream::pause()
{
	m_bPlaying = false;
}

void libsndAudioRecordingStream::read( Minim::MultiChannelBuffer & buffer )
{
	if ( m_sndFile )
	{
		const unsigned int outBufferSize = buffer.getBufferSize();
		//assert(outBufferSize <= m_bufferSize && "You requested to read more sample frames than this recording stream can read at one time.");
			   
		sf_count_t samplesToRead	= outBufferSize;
		sf_count_t totalSamplesRead	= 0;
		
		const int numberChannels = m_fileInfo.channels;
		buffer.setChannelCount( numberChannels );
		// start with silence, we may not be playing, in which case we should return silence
		// or we might read fewer samples than the full buffer, in which case the rest of it
		// should be filled with silence.
		buffer.makeSilence();
		
		if ( m_bPlaying )
		{
			while( samplesToRead > 0 )
			{
				sf_count_t readSize = samplesToRead < m_bufferSize ? samplesToRead : m_bufferSize;
				
				// heyo, we can just read floats and libsndfile converts for us!
				sf_count_t samplesRead = sf_read_float( m_sndFile, m_readBuffer, m_bufferSize );
				
				// 0 read means EOF
				if ( samplesRead == 0 )
				{
					// might need to wrap back to beginning loop point if looping,
					// but for now we just stop playing.
					m_bPlaying = false;
					return;
				}
				
				// and now we should be able to de-interleave our read buffer into buffer
				for(int c = 0; c < numberChannels; ++c)
				{
					float * channel = buffer.getChannel(c);
					for(int i = 0; i < samplesRead; ++i)
					{
						const int offset = (i * numberChannels) + c; 
						const float sample = m_readBuffer[offset];
						channel[totalSamplesRead + i ] = sample;
					}
				}
				
				samplesToRead	 -= samplesRead;
				totalSamplesRead += samplesRead;
			}
		}
	}
	else 
	{
		Minim::error("Tried to read from an unopened stream!");
	}
}
