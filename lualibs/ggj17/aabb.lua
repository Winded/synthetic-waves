-- AABB collision detection

include("tableext.lua");

local BBList = {};

local BB = {};

BB.__index = BB;
BB.__metatable = BB;
BB.__type = "BoundingBox";

function BB:position()
    return self._pos;
end

function BB:setPosition(position)
    self._pos = position;
end

function BB:size()
    return self._size;
end

function BB:setSize(size)
    self._size = size;
end

function BB:layers()
    return self._layers;
end

function BB:setLayers(layers)
    self._layers = layers;
end

function BB:isInSameLayer(otherBB)
    for _, layer in pairs(self._layers) do
        for _, layer2 in pairs(otherBB._layers) do
            if layer == layer2 then
                return true;
            end
        end
    end
    return false;
end

function BB:collidesWith(otherBB, ignoreLayers)
    if not ignoreLayers and not self:isInSameLayer(otherBB) then
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
            return true;
        end
    end
    return false;
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
        left = false,
        right = false,
        top = false,
        bottom = false,
    };

    self:setPosition(originalPos + delta);
    if not self:collidesWithAnything() then
        return cols;
    end

    self:setPosition(originalPos + luajogo.vec2(delta.x, 0));
    if not self:collidesWithAnything() then
        if delta.y < 0 then
            cols.bottom = true;
        else
            cols.top = true;
        end
        return cols;
    end

    self:setPosition(originalPos + luajogo.vec2(0, delta.y));
    if not self:collidesWithAnything() then
        if delta.x < 0 then
            cols.left = true;
        else
            cols.right = true;
        end
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
    return cols;
end

function BB:destroy()
    table.removeValue(BBList, self);
end

AABB = {};

function AABB.createBox(position, size, layers)
    local bb = {};
    setmetatable(bb, BB);

    bb._pos = position;
    bb._size = size;
    bb._layers = layers;

    table.insert(BBList, bb);
    return bb;
end