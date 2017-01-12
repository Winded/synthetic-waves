--[[
    LJ2D graphics library
]]

lj2d.graphics = {};

include("graphics/sprite.lua");

function lj2d.graphics._load()
    lj2d.graphics._context = luajogo.graphics.createContext();
    lj2d.graphics._context:setShaderParam("ScreenSize", luajogo.vec2(640, 480));
end

function lj2d.graphics._hookEvents()
    lj2d.window._window:addEventCallback("onWindowResize", function(w, data) 
        lj2d.graphics._context:setShaderParam("ScreenSize", luajogo.vec2(data.width, data.height));
    end);
end