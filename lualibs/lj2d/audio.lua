--[[
	LJ2D audio module
]]

lj2d.audio = {};

include("audio/clip.lua");
include("audio/source.lua");

function lj2d.audio._load()
    luajogo.audio.initialize();
end

function lj2d.audio._quit()
	luajogo.audio.quit();
end
