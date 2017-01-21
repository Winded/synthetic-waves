local PList = {};

local P = {};

P.__index = P;
P.__metatable = P;
P.__type = "Projectile";

function P:update(deltaTime)
    local move = self._moveDir * (self._moveSpeed * deltaTime);
    local cols = self._aabb:move(move);
    if cols.otherBB then
        if cols.otherBB:layer() == LAYER_ENEMY then
            local enemy = EnemySpawner.getEnemyFromBB(cols.otherBB);
            enemy:kill();
        elseif cols.otherBB:layer() == LAYER_PLAYER then
            if PLAYER._aabb == cols.otherBB then
                PLAYER:kill();
            end
        end
        -- explosion anim todo
        self:destroy();
    end

    self._sprite:setPosition(self._aabb:position());
end

function P:draw()
    self._sprite:draw();
end

function P:destroy()
    self._sprite:destroy();
    self._aabb:destroy();
    table.removeValue(PList, self);
end

Projectiles = {};

function Projectiles.create(position, direction, speed, layer)
    local invisColor = luajogo.color(0, 0, 0, 0);
    local texture = luajogo.texture.create({
        invisColor, invisColor, invisColor, invisColor, invisColor,
        invisColor, invisColor, invisColor, invisColor, invisColor,

        luajogo.color(255, 188, 0, 255),
        luajogo.color(255, 188, 0, 200),
        luajogo.color(255, 188, 0, 150),
        luajogo.color(255, 188, 0, 100),
        luajogo.color(255, 188, 0, 50),

        invisColor, invisColor, invisColor, invisColor, invisColor,
        invisColor, invisColor, invisColor, invisColor, invisColor,
    }, 5, 5);

    local p = {};
    setmetatable(p, P);

    local angle = math.atan2(direction.y, direction.x) * luajogo.RAD2DEG;
    p._sprite = lj2d.graphics.createSprite(texture, luajogo.vec2(40, 40), position, angle - 180);
    p._sprite:setPivot(luajogo.vec2(0, 0.5));

    p._aabb = AABB.createBox(position, luajogo.vec2(20, 20), layer);

    p._moveDir = direction:normalized();
    p._moveSpeed = speed;

    table.insert(PList, p);
    return p;
end

function Projectiles.update(deltaTime)
    for _, proj in pairs(PList) do
        proj:update(deltaTime);
    end
end

function Projectiles.draw()
    for _, proj in pairs(PList) do
        proj:draw();
    end
end

function Projectiles.quit()
    for _, proj in pairs(PList) do
        proj:destroy();
    end
end