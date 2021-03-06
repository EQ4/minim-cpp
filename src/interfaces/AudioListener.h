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

#ifndef AUDIOLISTENER_H
#define AUDIOLISTENER_H

#include "MultiChannelBuffer.h"

namespace Minim
{

	/** 
	  * Implementing AudioListener allows you to receive samples from an audio
	  * generating object in a callback fashion.
	  *
	  */
	class AudioListener
	{
	public:
		virtual void samples( const MultiChannelBuffer & buffer ) = 0;
	};

};

#endif // AUDIOLISTENER_H