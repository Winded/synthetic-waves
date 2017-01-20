local S = {};

S.__index = S;
S.__metatable = S;
S.__type = "LJ2DSource";

function S:sourceID()
	return self._sourceID;
end

function S:clip()
	return self._clip;
end

function S:setClip(clip)
	assert(type(clip) == "LJ2DClip", "Invalid clip");
	self._clip = clip;
	luajogo.audio.setSourceBuffer(self._sourceID, self._clip:bufferID());
end

function S:volume()
	return luajogo.audio.sourceVolume(self._sourceID);
end

function S:setVolume(volume)
	assert(type(volume) == "number", "Invalid volume");
	if volume > 1 then
		volume = 1;
	elseif volume < 0 then
		volume = 0;
	end
	luajogo.audio.setSourceVolume(self._sourceID, volume);
end

function S:pitch()
	return luajogo.audio.sourcePitch(self._sourceID);
end

function S:setPitch(pitch)
	assert(type(pitch) == "number", "Invalid pitch");
	luajogo.audio.setSourcePitch(self._sourceID, pitch);
end

function S:loop()
	return luajogo.audio.sourceLoop(self._sourceID);
end

function S:setLoop(loop)
	assert(type(loop) == "boolean", "Invalid loop");
	luajogo.audio.setSourceLoop(self._sourceID, loop);
end

function S:isPlaying()
	return luajogo.audio.sourcePlaying(self._sourceID);
end

function S:play()
	luajogo.audio.playSource(self._sourceID);
end

function S:pause()
	luajogo.audio.pauseSource(self._sourceID);
end

function S:stop()
	luajogo.audio.stopSource(self._sourceID);
end

function S:destroy()
	self._clip = nil;
	luajogo.audio.deleteSource(self._sourceID);
end

function lj2d.audio.createSource(clip)
	local source = {};
	setmetatable(source, S);

	source._sourceID = luajogo.audio.createSource();

	source:setClip(clip);
	source:setVolume(1);
	source:setPitch(1);
	source:setLoop(false);

	return source;
end