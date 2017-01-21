
include("aabb.lua");

local C = {};

C.__index = C;
C.__metatable = C;
C.__type = "Character";

function C:moveInput()
    return self._moveInput;
end

function C:setMoveInput(input)
    self._moveInput = input:normalized();
    print(input);
end

function C:position()
    return self._aabb:position();
end

function C:setPosition(pos)
    self._aabb:setPosition(pos);
end

function C:rotation()
    return self._sprite:rotation();
end

function C:setRotation(rotation)
    self._sprite:setRotation(rotation);
end

function C:size()
    return self._aabb:size();
end

function C:setSize(size)
    self._aabb:setSize(size);
    self._sprite:setSize(size);
end

function C:moveSpeed()
    return self._moveSpeed;
end

function C:setMoveSpeed(speed)
    self._moveSpeed = speed;
end

function C:currentAnimation()
    if self._moveInput ~= luajogo.vec2() then
        return self._anims.move;
    else
        return self._anims.idle;
    end
end

function C:_updateAnimation(deltaTime)
    local curAnim = self:currentAnimation();
    if curAnim ~= self._prevAnim then
        if self._prevAnim then
            self._prevAnim:stop();
        end
        self._prevAnim = curAnim;
        curAnim:play();
    end
    curAnim:update(deltaTime);
end

function C:update(deltaTime)
    local move = self._moveInput * (self._moveSpeed * deltaTime);
    if move ~= luajogo.vec2() then
        self._aabb:move(move);
    end
    self._sprite:setPosition(self._aabb:position());

    self:_updateAnimation(deltaTime);
end

function C:draw()
    self._sprite:draw();
end

function C:destroy()
    self._sprite:destroy();
end

Character = {};

function Character.create(sprite, animations, position, size, layers)
    local c = {};
    setmetatable(c, C);

    c._sprite = sprite;
    c._anims = animations;
    c._aabb = AABB.createBox(position, size, layers);

    c._moveSpeed = 10;
    c._moveInput = luajogo.vec2();

    c:setSize(size);

    return c;
end