
local EList = {};

EnemySpawner = {};

function EnemySpawner.spawnEnemy()
    local r = math.random(1, #(EnemySpawner._spawnPositions));
    local spawnPos = EnemySpawner._spawnPositions[r];

    local sprite = lj2d.graphics.createSprite(CHARACTER_SHEET);
    sprite:setSize(luajogo.vec2(46, 44));
    sprite:setPivot(luajogo.vec2(0.5, 0.5));
    local anims = {
        idle = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(169, 0), size = luajogo.vec2(46, 44) } },
        }),
        move = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(169, 0), size = luajogo.vec2(46, 44) } },
        }),
        dead = SpriteAnimation.create(sprite, {
            {time = 1, region = { position = luajogo.vec2(169, 0), size = luajogo.vec2(46, 44) } },
        }),
    };
    anims.idle:setLoop(true);
    anims.move:setLoop(true);

    local c = Character.create(sprite, anims, spawnPos, luajogo.vec2(60, 60), LAYER_ENEMY);
    table.insert(EList, c);
    return c;
end

function EnemySpawner.updateEnemyAI(enemy, deltaTime)
    if not enemy:isDead() then
        local dir = PLAYER:position() - enemy:position();
        local angle = math.atan2(dir.y, dir.x) * luajogo.RAD2DEG;
        enemy:setRotation(angle);

        if enemy._shootTime == nil then
            enemy._shootTime = 0;
        end
        enemy._shootTime = enemy._shootTime + deltaTime;
        if enemy._shootTime >= 1 and not PLAYER:isDead() then
            local enemyOffset = luajogo.vec2(50, -10);
            local m = luajogo.mat4x4();
            m:translateInPlace(luajogo.vec3(enemy:position().x, enemy:position().y, 0));
            m:rotateEuler(luajogo.vec3(0, 0, enemy:rotation()));
            local projPos = m * luajogo.vec4(enemyOffset.x, enemyOffset.y, 0, 1);
            Projectiles.create(luajogo.vec2(projPos.x, projPos.y), dir:normalized(), 500, LAYER_PROJECTILE_ENEMY);
            enemy._shootTime = 0;
        end
    end
end

function EnemySpawner.getEnemyFromBB(bb)
    for _, enemy in pairs(EList) do
        if enemy._aabb == bb then
            return enemy;
        end
    end
end

function EnemySpawner.load()
    EnemySpawner._timer = 0;
    EnemySpawner._spawnRatio = 5;
    EnemySpawner._spawnRatioChangeRate = -0.1;
    EnemySpawner._spawnPositions = {
        luajogo.vec2(0, 0),
        luajogo.vec2(200, 200),
    };
end

function EnemySpawner.update(deltaTime)
    for _, enemy in pairs(EList) do
        EnemySpawner.updateEnemyAI(enemy, deltaTime);
    end
    for _, enemy in pairs(EList) do
        enemy:update(deltaTime);
    end
end

function EnemySpawner.draw()
    for _, enemy in pairs(EList) do
        enemy:draw();
    end
end

function EnemySpawner.quit()
    for _, enemy in pairs(EList) do
        enemy:destroy();
    end
end