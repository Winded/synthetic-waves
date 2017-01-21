include("../lj2d/main.lua");

include("spriteanimation.lua");
include("aabb.lua");
include("character.lua");

local LAYER_WORLD = 1;
local LAYER_PLAYER = 2;
local LAYER_PROJECTILE = 3;
local LAYER_ENEMY = 4;

local player;

function lj2d.load()
    luajogo.assets.configure({
        assetSources = {
            {type = "folder", path = "../assets"}
        }
    });
    lj2d.window.setTitle("W A P O R W A V E");

    local texture = luajogo.texture.load(luajogo.assets.load("/topdown/Spritesheet/spritesheet_characters.png"));

    local sprite = lj2d.graphics.createSprite(texture);
    sprite:setPivot(luajogo.vec2(0.5, 0.5));

    local anims = {
        idle = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(215, 0), size = luajogo.vec2(46, 44) } },
        }),
        move = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(303, 43), size = luajogo.vec2(46, 44) } },
        }),
    };
    anims.idle:setLoop(true);
    anims.move:setLoop(true);

    player = Character.create(sprite, anims, luajogo.vec2(), luajogo.vec2(50, 43), {LAYER_WORLD, LAYER_PLAYER});
    player:setMoveSpeed(50);

    lj2d.graphics.camera.setPosition(luajogo.vec2(0, 0));
end

function lj2d.update(deltaTime)
    player:update(deltaTime);
end

function lj2d.keyDown(keycode)
    if keycode == "W" then
        player:setMoveInput(luajogo.vec2(player:moveInput().x, 1));
    elseif keycode == "S" then
        player:setMoveInput(luajogo.vec2(player:moveInput().x, -1));
    end
    if keycode == "D" then
        player:setMoveInput(luajogo.vec2(1, player:moveInput().y));
    elseif keycode == "A" then
        player:setMoveInput(luajogo.vec2(-1, player:moveInput().y));
    end
end

function lj2d.keyUp(keycode)
    if keycode == "W" or keycode == "S" then
        player:setMoveInput(luajogo.vec2(player:moveInput().x, 0));
    end
    if keycode == "D" or keycode == "A" then
        player:setMoveInput(luajogo.vec2(0, player:moveInput().y));
    end
end

function lj2d.mouseMove(delta)
    local worldPos = lj2d.graphics.camera.screenToWorldPosition(lj2d.input.mousePosition());
    local delta = worldPos - player:position();
    local angle = math.atan2(delta.y, delta.x) * luajogo.RAD2DEG;
    player:setRotation(angle);
end

function lj2d.draw()
    lj2d.graphics.clear(luajogo.color(0, 0, 0, 255));

    player:draw();
end

function lj2d.quit()
    player:destroy();
end