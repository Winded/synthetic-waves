--[[
    LJ2D sprites
]]

local VERTEX_CODE = [[
#version 330

in vec2 position;
in vec2 uvCoordinates;

out vec2 UV;

uniform mat4 LocalToWorldMatrix;
uniform vec2 ScreenSize;

void main()
{
    vec4 pos = LocalToWorldMatrix * vec4(position.xy, 0, 1);
    pos.x = ((pos.x / ScreenSize.x) * 2 - 1);
    pos.y = -((pos.y / ScreenSize.y) * 2 - 1);
    gl_Position = vec4(pos.xyz, 1);
    UV = uvCoordinates;
}
]];
local FRAGMENT_CODE = [[
#version 330

in vec2 UV;

out vec4 outColor;

uniform sampler2D Texture1;
uniform vec4 BaseColor;

void main()
{
    outColor = texture2D(Texture1, UV) * BaseColor;
}
]];

local spriteShader = nil;
local spriteVertexArray = nil;

local S = {};

S.__index = S;
S.__metatable = S;
S.__type = "LJ2DSprite";

function S:_updateLToWMatrix()
    if not self._gobj or not self._gobj:isValid() then
        return;
    end

    local m = luajogo.mat4x4();
    m:translateInPlace(luajogo.vec3(self._pos.x, self._pos.y, 0));
    m:scale(luajogo.vec3(self._size.x, self._size.y, 1));
    m:rotateEuler(luajogo.vec3(0, 0, self._rot));
    self._gobj:setShaderParam("LocalToWorldMatrix", m);
    self._gobj:setShaderParam("BaseColor", self._color);
end

function S:texture()
    return self._texture;
end

function S:setTexture(texture)
    assert(type(texture) == "texture", "Invalid texture");

    self._texture = texture;

    if self._gtex and self._gtex:isValid() then
        self._gtex:destroy();
    end
    self._gtex = lj2d.graphics._context:createTexture(texture);
    self._gobj:setTexture(self._gtex);
end

function S:position()
    return self._pos;
end

function S:setPosition(pos)
    assert(type(pos) == "vec2", "Position must be of type vec2");

    self._pos = pos;
    self:_updateLToWMatrix();
end

function S:rotation()
    return self._rot;
end

function S:setRotation(rot)
    assert(type(rot) == "number", "Rotation must be of type number");

    self._rot = rot;
    self:_updateLToWMatrix();
end

function S:size()
    return self._size;
end

function S:setSize(size)
    assert(type(size) == "vec2", "Size must be of type vec2");

    self._size = size
    self:_updateLToWMatrix();
end

function S:destroy()
    if self._gobj and self._gobj:isValid() then
        self._gobj:destroy();
    end
    if self._gtex and self._gtex:isValid() then
        self._gtex:destroy();
    end
end

function lj2d.graphics.createSprite(texture, size, position, rotation)
    assert(type(texture) == "texture", "Invalid texture");

    if not spriteShader then
        local vs = lj2d.graphics._context:createShader(VERTEX_CODE, luajogo.VERTEX_SHADER);
        local fs = lj2d.graphics._context:createShader(FRAGMENT_CODE, luajogo.FRAGMENT_SHADER);
        spriteShader = lj2d.graphics._context:createShaderProgram(vs, fs);
    end
    if not spriteVertexArray then
        local va = lj2d.graphics._context:createVertexArray({
            0, 0, 0, 0,
            0, 1, 0, 1,
            1, 1, 1, 1,
            1, 0, 1, 0
        }, {
            0, 1, 3,
            1, 2, 3,
        });
        va:setAttribute(0, 2);
        va:setAttribute(1, 2);
        spriteVertexArray = va;
    end

    local sprite = {};
    setmetatable(sprite, S);

    local w, h = texture:size();

    sprite._texture = texture;
    sprite._size = size or luajogo.vec2(w, h);
    sprite._pos = position or luajogo.vec3();
    sprite._rot = rotation or 0;
    sprite._color = luajogo.color("white");

    sprite._gtex = lj2d.graphics._context:createTexture(texture);
    sprite._gobj = lj2d.graphics._context:createObject();
    sprite._gobj:setShaderProgram(spriteShader);
    sprite._gobj:setVertexArray(spriteVertexArray);
    sprite._gobj:setTexture(sprite._gtex);

    sprite:_updateLToWMatrix();

    lj2d.graphics._context:refreshDrawOrder();

    return sprite;
end