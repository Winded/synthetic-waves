--[[
    LJ2D

    Luajogo 2D game library
]]

lj2d = {};

include("window.lua");
include("input.lua");
include("graphics.lua");
include("events.lua");

function main()
    lj2d.window._load();
    lj2d.graphics._load();

    lj2d.load();

    lj2d.window._hookEvents();
    lj2d.input._hookEvents();
    lj2d.graphics._hookEvents();

    lj2d.window._open();

    local dt
    while not lj2d.window._shouldClose() do
        lj2d.window._pollEvents();

        dt = lj2d.window.deltaTime();
        -- TODO

        lj2d.update(dt);

        lj2d.draw();

        lj2d.window._swapBuffers();
    end

    lj2d.quit();
end

return lj2d;