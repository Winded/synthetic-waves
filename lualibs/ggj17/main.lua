include("../lj2d/main.lua");

include("spriteanimation.lua");
include("aabb.lua");
include("character.lua");
include("world.lua");
include("projectiles.lua");
include("enemyspawner.lua");

CHARACTER_SHEET = nil;
PLAYER = nil;
local enemy;

local function createPlayer()
    local sprite = lj2d.graphics.createSprite(CHARACTER_SHEET);
    sprite:setSize(luajogo.vec2(46, 44));
    sprite:setPivot(luajogo.vec2(0.5, 0.5));

    local anims = {
        idle = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(215, 0), size = luajogo.vec2(46, 44) } },
        }),
        move = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(215, 0), size = luajogo.vec2(46, 44) } },
        }),
        dead = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(215, 0), size = luajogo.vec2(46, 44) } },
        }),
    };
    anims.idle:setLoop(true);
    anims.move:setLoop(true);

    PLAYER = Character.create(sprite, anims, luajogo.vec2(), luajogo.vec2(60, 60), LAYER_PLAYER);
    PLAYER:setMoveSpeed(200);
end

function lj2d.load()
    luajogo.assets.configure({
        assetSources = {
            {type = "folder", path = "../assets"}
        }
    });
    lj2d.window.setTitle("W A P O R W A V E");

    CHARACTER_SHEET = luajogo.texture.load(luajogo.assets.load("/topdown/Spritesheet/spritesheet_characters.png"));

    World.load();
    EnemySpawner.load();

    createPlayer();

    EnemySpawner.spawnEnemy();

    lj2d.graphics.camera.setPosition(luajogo.vec2(0, 0));
end

function lj2d.update(deltaTime)
    EnemySpawner.update(deltaTime);

    PLAYER:update(deltaTime);

    if not PLAYER:isDead() then
        local worldPos = lj2d.graphics.camera.screenToWorldPosition(lj2d.input.mousePosition());
        local delta = worldPos - PLAYER:position();
        local angle = math.atan2(delta.y, delta.x) * luajogo.RAD2DEG;
        PLAYER:setRotation(angle);
    end

    local cam = lj2d.graphics.camera;
    cam.setPosition(cam.position() + (PLAYER:position() - cam.position()) / 8);

    Projectiles.update(deltaTime);
end

function lj2d.keyDown(keycode)
    if keycode == "W" then
        PLAYER:setMoveInput(luajogo.vec2(PLAYER:moveInput().x, 1));
    elseif keycode == "S" then
        PLAYER:setMoveInput(luajogo.vec2(PLAYER:moveInput().x, -1));
    end
    if keycode == "D" then
        PLAYER:setMoveInput(luajogo.vec2(1, PLAYER:moveInput().y));
    elseif keycode == "A" then
        PLAYER:setMoveInput(luajogo.vec2(-1, PLAYER:moveInput().y));
    end
end

function lj2d.keyUp(keycode)
    if keycode == "W" or keycode == "S" then
        PLAYER:setMoveInput(luajogo.vec2(PLAYER:moveInput().x, 0));
    end
    if keycode == "D" or keycode == "A" then
        PLAYER:setMoveInput(luajogo.vec2(0, PLAYER:moveInput().y));
    end
end

function lj2d.mouseDown(button, pos, numClicks)
    if button == 1 then
        local worldPos = lj2d.graphics.camera.screenToWorldPosition(lj2d.input.mousePosition());
        local delta = worldPos - PLAYER:position();
        local playerOffset = luajogo.vec2(50, -10);
        local m = luajogo.mat4x4();
        m:translateInPlace(luajogo.vec3(PLAYER:position().x, PLAYER:position().y, 0));
        m:rotateEuler(luajogo.vec3(0, 0, PLAYER:rotation()));
        local projPos = m * luajogo.vec4(playerOffset.x, playerOffset.y, 0, 1);
        Projectiles.create(luajogo.vec2(projPos.x, projPos.y), delta:normalized(), 500, LAYER_PROJECTILE_PLAYER);
    end
end

function lj2d.mouseMove(delta)
end

function lj2d.draw()
    lj2d.graphics.clear(luajogo.color(0, 0, 0, 255));

    World.draw();

    EnemySpawner.draw();

    PLAYER:draw();

    Projectiles.draw();
end

function lj2d.quit()
    PLAYER:destroy();
    EnemySpawner.load();
    Projectiles.quit();
    World.quit();
end