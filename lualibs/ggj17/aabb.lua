-- AABB collision detection

include("tableext.lua");

LAYER_WORLD = 1;
LAYER_PLAYER = 2;
LAYER_PROJECTILE_PLAYER = 3;
LAYER_PROJECTILE_ENEMY = 4;
LAYER_ENEMY = 5;
LAYER_DEAD = 6;

local colMatrix = {
    {LAYER_PLAYER, LAYER_WORLD}, -- Player collides with world
    {LAYER_ENEMY, LAYER_WORLD}, -- Enemies collide with world
    {LAYER_PROJECTILE_PLAYER, LAYER_WORLD}, -- Projectiles collide with world
    {LAYER_PROJECTILE_ENEMY, LAYER_WORLD},
    {LAYER_PROJECTILE_PLAYER, LAYER_ENEMY},
    {LAYER_PROJECTILE_ENEMY, LAYER_PLAYER},
    {LAYER_WORLD, LAYER_WORLD}, -- World collides with world
};

local BBList = {};

local BB = {};

BB.__index = BB;
BB.__metatable = BB;
BB.__type = "BoundingBox";

function BB:position()
    return self._pos * 1;
end

function BB:setPosition(position)
    self._pos = position * 1;
end

function BB:size()
    return self._size * 1;
end

function BB:setSize(size)
    self._size = size * 1;
end

function BB:layer()
    return self._layer;
end

function BB:setLayer(layers)
    self._layer = layers;
end

function BB:layersCollide(otherBB)
    for _, layers in pairs(colMatrix) do
        if (self._layer == layers[1] and otherBB._layer == layers[2]) or (otherBB._layer == layers[1] and self._layer == layers[2]) then
            return true;
        end
    end
    return false;
end

function BB:collidesWith(otherBB, ignoreLayers)
    if not ignoreLayers and not self:layersCollide(otherBB) then
        return false;
    end

    return self._pos.x < otherBB._pos.x + otherBB._size.x and 
            self._pos.x + self._size.x > otherBB._pos.x and 
            self._pos.y < otherBB._pos.y + otherBB._size.y and
            self._pos.y + self._size.y > otherBB._pos.y;
end

function BB:collidesWithAnything(ignoreLayers)
    for k, v in pairs(BBList) do
        if v ~= self and self:collidesWith(v, ignoreLayers) then
            return v;
        end
    end
    return nil;
end

function BB:collisions()
    local cols = {};
    for k, v in pairs(BBList) do
        if v ~= self and self:collidesWith(v) then
            table.insert(cols, v);
        end
    end
    return cols;
end

function BB:move(delta)
    local originalPos = self:position();
    local cols = {
        otherBB = nil,
        left = false,
        right = false,
        top = false,
        bottom = false,
    };

    self:setPosition(originalPos + delta);
    local bb = self:collidesWithAnything();
    if not bb then
        return cols;
    end

    self:setPosition(originalPos + luajogo.vec2(delta.x, 0));
    local prevBB = bb;
    local bb = self:collidesWithAnything();
    if not bb then
        if delta.y < 0 then
            cols.bottom = true;
        else
            cols.top = true;
        end
        cols.otherBB = prevBB;
        return cols;
    end

    self:setPosition(originalPos + luajogo.vec2(0, delta.y));
    local prevBB = bb;
    local bb = self:collidesWithAnything();
    if not bb then
        if delta.x < 0 then
            cols.left = true;
        else
            cols.right = true;
        end
        cols.otherBB = prevBB;
        return cols;
    end

    self:setPosition(originalPos);
    if delta.x < 0 then
        cols.left = true;
    else
        cols.right = true;
    end
    if delta.y < 0 then
        cols.bottom = true;
    else
        cols.top = true;
    end
    cols.otherBB = bb;
    return cols;
end

function BB:destroy()
    table.removeValue(BBList, self);
end

AABB = {};

function AABB.createBox(position, size, layer)
    local bb = {};
    setmetatable(bb, BB);

    bb._pos = position;
    bb._size = size;
    bb._layer = layer;

    table.insert(BBList, bb);
    return bb;
end