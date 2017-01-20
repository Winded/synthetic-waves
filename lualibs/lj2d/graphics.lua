--[[
    LJ2D graphics library
]]

lj2d.graphics = {};

include("graphics/sprite.lua");

function lj2d.graphics._load()
end

function lj2d.graphics._quit()
end

function lj2d.graphics._hookEvents()
end

function lj2d.graphics.clear(color)
    luajogo.graphics.clear(color);
end