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
    local m = luajogo.mat4x4();
    m:translateInPlace(luajogo.vec3(self._pos.x, self._pos.y, 0));
    m:scale(luajogo.vec3(self._size.x, self._size.y, 1));
    m:rotateEuler(luajogo.vec3(0, 0, self._rot));
    self._lToWMat = m;
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

function S:draw()
    luajogo.graphics.useShaderProgram(spriteShader);
    luajogo.graphics.useTexture(self._gtex);

    luajogo.graphics.setShaderParam(spriteShader, "LocalToWorldMatrix", self._lToWMat);
    luajogo.graphics.setShaderParam(spriteShader, "ScreenSize", lj2d.window.size());
    luajogo.graphics.setShaderParam(spriteShader, "BaseColor", self._color);

    luajogo.graphics.useVertexArray(spriteVertexArray);

    luajogo.graphics.draw();
end

function S:destroy()
    if self._gtex then
        luajogo.graphics.deleteTexture(self._gtex);
        self._gtex = nil;
    end
end

function lj2d.graphics.createSprite(texture, size, position, rotation)
    assert(type(texture) == "texture", "Invalid texture");

    if not spriteShader then
        local vs = luajogo.graphics.createShader(VERTEX_CODE, luajogo.graphics.VERTEX_SHADER);
        local fs = luajogo.graphics.createShader(FRAGMENT_CODE, luajogo.graphics.FRAGMENT_SHADER);
        spriteShader = luajogo.graphics.createShaderProgram(vs, fs);
    end
    if not spriteVertexArray then
        local va = luajogo.graphics.createVertexArray({
            0, 0, 0, 0,
            0, 1, 0, 1,
            1, 1, 1, 1,
            1, 0, 1, 0
        }, {
            0, 1, 3,
            1, 2, 3,
        });
        luajogo.graphics.setVertexArrayAttribute(va, 0, 2);
        luajogo.graphics.setVertexArrayAttribute(va, 1, 2);
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

    sprite._gtex = luajogo.graphics.createTexture(texture);

    sprite:_updateLToWMatrix();

    return sprite;
end