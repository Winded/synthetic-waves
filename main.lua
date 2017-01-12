-- Test script

local VERTEX_CODE = [[
#version 330

in vec3 position;
in vec2 uvCoordinates;

out vec2 UV;

uniform mat4 WorldToViewportMatrix;
uniform mat4 LocalToWorldMatrix;

void main()
{
    gl_Position = WorldToViewportMatrix * LocalToWorldMatrix * vec4(position.xyz, 1.0);
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
local FRAGMENT_FONT_CODE = [[
#version 330

in vec2 UV;

out vec4 outColor;

uniform sampler2D Texture1;
uniform vec4 BaseColor;

void main()
{
    vec4 c = texture2D(Texture1, UV) * BaseColor;
    outColor = vec4(c.r, c.r, c.r, c.r);
}
]];

local BOX = {
    vbuf = {
        -0.5, -0.5, 0.5, 0, 0,
        -0.5, -0.5, -0.5, 0, 0,
        -0.5, 0.5, -0.5, 0, 1,
        -0.5, 0.5, 0.5, 0, 1,
        0.5, -0.5, 0.5, 1, 0,
        0.5, -0.5, -0.5, 1, 0,
        0.5, 0.5, -0.5, 1, 1,
        0.5, 0.5, 0.5, 1, 1,
    },
    ebuf = {
        3, 2, 1,
        3, 0, 1,
        1, 5, 4,
        1, 0, 4,
        2, 6, 5,
        2, 1, 5,
        7, 6, 2,
        7, 3, 2,
        4, 7, 3,
        4, 0, 3,
        5, 6, 7,
        5, 4, 7,
    }
};

local QUAD = {
    vbuf = {
        -0.5, 0.5, 0, 0, 0,
        0.5, 0.5, 0, 0, 1,
        0.5, -0.5, 0, 1, 1,
        -0.5, -0.5, 0, 1, 0
    },
    ebuf = {
        0, 1, 3,
        1, 2, 3
    }
};

include("lualibs/lj2d/main.lua");

local sprite;
local spriteSize;

function lj2d.load()
    luajogo.assets.configure({
        assetSources = {
            {type = "folder", path = "../assets"}
        }    
    });
    lj2d.window.setTitle("LJ2D demo");
    lj2d.window.setClearColor(luajogo.color(150, 150, 150, 255));

    local texAsset = luajogo.assets.load("/tileset.png");
    local tex = luajogo.texture.load(texAsset);
    sprite = lj2d.graphics.createSprite(tex);
    spriteSize = sprite:size();
    sprite:setPosition(luajogo.vec2(50, 50));
end

function lj2d.update(deltaTime)
    spriteSize = spriteSize + luajogo.vec2(2, 2) * deltaTime;
    sprite:setSize(luajogo.vec2(math.floor(spriteSize.x), math.floor(spriteSize.y)))
    sprite:setRotation(sprite:rotation() + deltaTime * 45);
end

function lj2d.quit()
    if sprite then
        sprite:destroy();
    end
end