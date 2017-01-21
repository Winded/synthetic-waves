local S = {};

S.__index = S;
S.__metatable = S;
S.__type = "SpriteAnimation";

function S:_updateSprite()
    local frame = nil;
    local t = 0;
    for i, v in ipairs(self._frames) do
        t = t + v.time;
        if t >= self._time then
            frame = v;
            break;
        end
    end

    assert(frame ~= nil, "No frame found!");

    self._sprite:setTextureRegion(frame.region);
end

function S:duration()
    local d = 0;
    for i, v in ipairs(self._frames) do
        d = d + v.time;
    end
    return d;
end

function S:time()
    return self._time;
end

function S:setTime(time)
    self._time = time;
    if self._time > self:duration() then
        self._time = self:duration();
    elseif self._time < 0 then 
        self._time = 0;
    end
    self:_updateSprite();
end

function S:loop()
    return self._loop;
end

function S:setLoop(loop)
    self._loop = loop;
end

function S:sprite()
    return self._sprite;
end

function S:setSprite(sprite)
    assert(type(sprite) == "LJ2DSprite", "Invalid sprite");
    self._sprite = sprite;
end

function S:update(deltaTime)
    if self:isPlaying() then
        self._time = self._time + deltaTime;
        if self._time > self:duration() then
            if self:loop() then
                self._time = 0;
            else
                self._time = self:duration();
                self:pause();
            end
        end
    end
    self:_updateSprite();
end

function S:isPlaying()
    return self._playing;
end

function S:play()
    self._playing = true;
end

function S:pause()
    self._playing = false;
end

function S:stop()
    self._playing = false;
    self:setTime(0);
end

function S:destroy()
    self._sprite:destroy();
    self._sprite = nil;
end

SpriteAnimation = {};

function SpriteAnimation.create(sprite, frames)
    assert(type(sprite) == "LJ2DSprite", "Invalid sprite");
    assert(type(frames) == "table", "Frame table is not a table?");

    local sAnim = {};
    setmetatable(sAnim, S);

    sAnim._sprite = sprite;
    sAnim._frames = frames;
    sAnim._loop = false;
    sAnim:setTime(0);

    return sAnim;
end