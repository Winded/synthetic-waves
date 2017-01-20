local C = {};

C.__index = C;
C.__metatable = C;
C.__type = "LJ2DClip";

function C:assetPath()
	return self._asset:path();
end

function C:bufferID()
	return self._bufID;
end

function C:update(asset)
	assert(type(asset) == "asset", "Invalid asset");

	self._asset = asset;
	luajogo.audio.updateBuffer(self._bufID, asset);
end

function C:destroy()
	self._asset = nil;
	luajogo.audio.deleteBuffer(self._bufID);
end

function lj2d.audio.createClip(asset)
	assert(type(asset) == "asset", "Invalid asset");

	local clip = {};
	setmetatable(clip, C);

	clip._asset = asset;
	clip._bufID = luajogo.audio.createBuffer(asset);
	return clip;
end