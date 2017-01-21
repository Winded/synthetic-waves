include("../lj2d/main.lua");

include("spriteanimation.lua");
include("aabb.lua");

local s1, s2;

local b1, b2;

function lj2d.load()
    luajogo.assets.configure({
        assetSources = {
            {type = "folder", path = "../assets"}
        }
    });
    lj2d.window.setTitle("W A P O R W A V E");

    local texture = luajogo.texture.load(luajogo.assets.load("/tileset.png"));

    local sprite = lj2d.graphics.createSprite(texture);
    sprite:setPivot(luajogo.vec2(0.5, 0.5));
    s1 = SpriteAnimation.create(sprite, {
        { time = 1, region = { position = luajogo.vec2(0, 0), size = luajogo.vec2(16, 16) } },
        { time = 1, region = { position = luajogo.vec2(16, 16), size = luajogo.vec2(16, 16) } },
    });
    s1:setLoop(true);
    s1:play();
    
    sprite = lj2d.graphics.createSprite(texture);
    sprite:setPivot(luajogo.vec2(0.5, 0.5));
    s2 = SpriteAnimation.create(sprite, {
        { time = 1.5, region = { position = luajogo.vec2(0, 0), size = luajogo.vec2(16, 16) } },
        { time = 0.5, region = { position = luajogo.vec2(16, 16), size = luajogo.vec2(16, 16) } },
    });
    s2:setLoop(true);
    s2:play();

    b1 = AABB.createBox(luajogo.vec2(200, 200), luajogo.vec2(100, 100), {1});
    b2 = AABB.createBox(luajogo.vec2(300, 300), luajogo.vec2(100, 100), {1});
end

function lj2d.update(deltaTime)
    s1:update(deltaTime);
    s2:update(deltaTime);

    s1:sprite():setPosition(b1:position());
    s1:sprite():setSize(b1:size());
    s2:sprite():setPosition(b2:position());
    s2:sprite():setSize(b2:size());
end

function lj2d.keyDown(keycode)
    if keycode == "W" then
        b1:move(luajogo.vec2(0, 10));
    elseif keycode == "S" then
        b1:move(luajogo.vec2(0, -10));
    elseif keycode == "D" then
        b1:move(luajogo.vec2(10, 0));
    elseif keycode == "A" then
        b1:move(luajogo.vec2(-10, 0));
    end
end

function lj2d.draw()
    lj2d.graphics.clear(luajogo.color(0, 0, 0, 255));

    s1:sprite():draw();
    s2:sprite():draw();
end

function lj2d.quit()
    s1:destroy();
end