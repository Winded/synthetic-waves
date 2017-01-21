include("../lj2d/main.lua");

include("spriteanimation.lua");

local spriteAnim;

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
    sprite:setPosition(luajogo.vec2(400, 300));
    spriteAnim = SpriteAnimation.create(sprite, {
        { time = 1, region = { position = luajogo.vec2(0, 0), size = luajogo.vec2(16, 16) } },
        { time = 1, region = { position = luajogo.vec2(16, 16), size = luajogo.vec2(16, 16) } },
    });
    spriteAnim:setLoop(true);
    spriteAnim:play();
end

function lj2d.update(deltaTime)
    spriteAnim:update(deltaTime);
end

function lj2d.keyDown(keycode)

end

function lj2d.draw()
    lj2d.graphics.clear(luajogo.color(0, 0, 0, 255));

    spriteAnim:sprite():draw();
end

function lj2d.quit()
    spriteAnim:destroy();
end