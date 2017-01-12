--[[
    LJ2D window methods
]]

lj2d.input = {};

lj2d.input._keys = {};

lj2d.input._mouseButtons = {};
lj2d.input._mousePos = luajogo.vec2();

function lj2d.input._hookEvents()
    lj2d.window._window:addEventCallback("onKeyDown", function(w, data)
        lj2d.input._keys[data.key] = true;
        lj2d.keyDown(data.key);
    end);
    lj2d.window._window:addEventCallback("onKeyUp", function(w, data)
        lj2d.input._keys[data.key] = false;
        lj2d.keyUp(data.key);
    end);
    lj2d.window._window:addEventCallback("onMouseDown", function(w, data)
        lj2d.input._mouseButtons[data.button] = true;
        lj2d.mouseDown(data.button, luajogo.vec2(data.x, data.y), data.clicks);
    end);
    lj2d.window._window:addEventCallback("onMouseUp", function(w, data)
        lj2d.input._mouseButtons[data.button] = false;
        lj2d.mouseUp(data.button, luajogo.vec2(data.x, data.y), data.clicks);
    end);
    lj2d.window._window:addEventCallback("onMouseMove", function(w, data)
        lj2d.input._mousePos.x = data.x;
        lj2d.input._mousePos.y = data.y;
        lj2d.mouseMove(luajogo.vec2(data.xrel, data.yrel));
    end);
end

function lj2d.input.isKeyPressed(key)
    if lj2d.input._keys[key] then
        return true;
    else
        return false;
    end
end

function lj2d.input.isMousePressed(button)
    if lj2d.input._mouseButtons[button] then
        return true;
    else
        return false;
    end
end

function lj2d.input.mousePosition()
    return lj2d.input._mousePos * 1;
end