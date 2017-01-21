--[[
    LJ2D sprites
]]

local VERTEX_CODE = [[
#version 330

in vec2 position;
in vec2 uvCoordinates;

out vec2 UV;

uniform mat4 WorldToViewportMatrix;
uniform mat4 LocalToWorldMatrix;

void main()
{
    gl_Position = WorldToViewportMatrix * LocalToWorldMatrix * vec4(position.xy, 0, 1);
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

function S:_updateVertices()
    local minPos = -self._pivot;
    local maxPos = -self._pivot + luajogo.vec2(1, 1);
    local texWidth, texHeight = self._texture:size();
    local minTexCoords = luajogo.vec2(self._texRegion.position.x / texWidth, self._texRegion.position.y / texHeight);
    local maxTexCoords = luajogo.vec2((self._texRegion.position.x + self._texRegion.size.x) / texWidth, (self._texRegion.position.y + self._texRegion.size.y) / texHeight);

    local verts = {
        minPos.x, minPos.y, minTexCoords.x, maxTexCoords.y,
        minPos.x, maxPos.y, minTexCoords.x, minTexCoords.y,
        maxPos.x, maxPos.y, maxTexCoords.x, minTexCoords.y,
        maxPos.x, minPos.y, maxTexCoords.x, maxTexCoords.y
    }

    luajogo.graphics.updateVertexArray(self._gva, verts, {
        0, 1, 3,
        1, 2, 3,
    });
    luajogo.graphics.setVertexArrayAttribute(self._gva, 0, 2);
    luajogo.graphics.setVertexArrayAttribute(self._gva, 1, 2);
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

    self._size = size;
    self:_updateLToWMatrix();
end

function S:pivot()
    return self._pivot;
end

function S:setPivot(pivot)
    assert(type(pivot) == "vec2", "Pivot must be of type vec2");

    self._pivot = pivot;
    self:_updateVertices();
end

function S:textureRegion()
    return self._texRegion;
end

function S:setTextureRegion(region)
    assert(type(region.position) == "vec2", "Invalid region position");
    assert(type(region.size) == "vec2", "Invalid region size");

    self._texRegion = region;
    self:_updateVertices();
end

function S:resetTextureRegion()
    local texWidth, texHeight = self._texture:size();
    self._texRegion = {
        position = luajogo.vec2(),
        size = luajogo.vec2(texWidth, texHeight),
    };
    self:_updateVertices();
end

function S:draw()
    luajogo.graphics.useShaderProgram(spriteShader);
    luajogo.graphics.useTexture(self._gtex);
    luajogo.graphics.useVertexArray(self._gva);

    luajogo.graphics.setShaderParam(spriteShader, "WorldToViewportMatrix", lj2d.graphics.camera.viewMatrix());
    luajogo.graphics.setShaderParam(spriteShader, "LocalToWorldMatrix", self._lToWMat);
    luajogo.graphics.setShaderParam(spriteShader, "BaseColor", self._color);

    luajogo.graphics.draw();
end

function S:destroy()
    if self._gtex then
        luajogo.graphics.deleteTexture(self._gtex);
        self._gtex = nil;
    end
    if self._gva then
        luajogo.graphics.deleteVertexArray(self._gva);
        self._gva = nil;
    end
end

function lj2d.graphics.createSprite(texture, size, position, rotation)
    assert(type(texture) == "texture", "Invalid texture");

    if not spriteShader then
        local vs = luajogo.graphics.createShader(VERTEX_CODE, luajogo.graphics.VERTEX_SHADER);
        local fs = luajogo.graphics.createShader(FRAGMENT_CODE, luajogo.graphics.FRAGMENT_SHADER);
        spriteShader = luajogo.graphics.createShaderProgram(vs, fs);
    end

    local sprite = {};
    setmetatable(sprite, S);

    local w, h = texture:size();

    sprite._texture = texture;
    sprite._size = size or luajogo.vec2(w, h);
    sprite._pos = position or luajogo.vec2();
    sprite._rot = rotation or 0;
    sprite._pivot = luajogo.vec2();
    sprite._color = luajogo.color("white");

    sprite._gtex = luajogo.graphics.createTexture(texture);

    sprite._gva = luajogo.graphics.createVertexArray({
        0, 0, 0, 1,
        0, 1, 0, 0,
        1, 1, 1, 0,
        1, 0, 1, 1
    }, {
        0, 1, 3,
        1, 2, 3,
    });
    luajogo.graphics.setVertexArrayAttribute(sprite._gva, 0, 2);
    luajogo.graphics.setVertexArrayAttribute(sprite._gva, 1, 2);

    sprite:_updateLToWMatrix();
    sprite:resetTextureRegion();

    return sprite;
end