--[[
    LJ2D window methods
]]

lj2d.window = {};

function lj2d.window._load()
    local w = luajogo.window.create(640, 480, "LJ2D window");
    if not w:isValid() then
        error("Window is invalid");
    end

    w:setGraphicsContext(lj2d.graphics._context);
    w:setClearColor(luajogo.color("black"));

    lj2d.window._window = w;
end

function lj2d.window._hookEvents()
    lj2d.window._window:addEventCallback("onWindowResize", function(w, data) 
        lj2d.resize(data.width, data.height);
    end);
end

function lj2d.window._open()
    lj2d.window._window:open();
end

function lj2d.window._shouldClose()
    return lj2d.window._window:shouldClose();
end

function lj2d.window._pollEvents()
    lj2d.window._window:pollEvents()
end

function lj2d.window._draw()
    lj2d.window._window:draw();
end

function lj2d.window.time()
    return lj2d.window._window:time();
end

function lj2d.window.deltaTime()
    return lj2d.window._window:deltaTime();
end

function lj2d.window.size()
    local w, h = lj2d.window._window:size();
    return luajogo.vec2(w, h);
end

function lj2d.window.setSize(width, height)
    if type(width) == "vec2" then
        lj2d.window._window:setSize(width.x, width.y);
    else
        lj2d.window._window:setSize(width, height);
    end
end

function lj2d.window.setClearColor(color)
    lj2d.window._window:setClearColor(color);
end

function lj2d.window.title()
    return lj2d.window._window:title();
end

function lj2d.window.setTitle(title)
    lj2d.window._window:setTitle(title);
end