--[[
    LJ2D graphics library
]]

lj2d.graphics = {};

include("graphics/camera.lua");
include("graphics/sprite.lua");

function lj2d.graphics._load()
    lj2d.graphics.camera.reset();
end

function lj2d.graphics._quit()
end

function lj2d.graphics._hookEvents()
    lj2d.graphics.camera._hookEvents();
end

function lj2d.graphics.clear(color)
    luajogo.graphics.clear(color);
end