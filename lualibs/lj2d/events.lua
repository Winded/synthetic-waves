--[[
    LJ2D events. Everything is declared so we don't need to check for nil functions
]]

-- Called when the game starts, before update loop starts
function lj2d.load() end

-- Called once every frame
function lj2d.update(deltaTime) end

-- Called when the game closes
function lj2d.quit() end

-- Called when window is resized
function lj2d.resize(width, height) end

-- Called when key is pressed
function lj2d.keyDown(keycode) end
-- Called when key is released
function lj2d.keyUp(keycode) end

-- Called when mouse button is pressed
function lj2d.mouseUp(button, pos, numClicks) end
-- Called when mouse button is released
function lj2d.mouseDown(button, pos, numClicks) end

-- Called when mouse is moved
function lj2d.mouseMove(delta) end