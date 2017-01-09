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

local WIDTH = 640;
local HEIGHT = 480;
local FOV = 60;

local rotateBox = false;
local rotateDir = luajogo.vec3(1, 1, 1);

function makeWorldToVPMat(camPos, camRot, screenWidth, screenHeight, fov)
    local projMat = luajogo.mat4x4();
    local aspectRatio = screenWidth / screenHeight;
    projMat:perspective(fov, aspectRatio, 0.5, 500);

    local lToW = luajogo.mat4x4();
    lToW:translate(camPos);
    lToW:rotateEuler(camRot);
    local forward = lToW * luajogo.vec4(0, 0, -1, 1);
    forward = luajogo.vec3(forward.x, forward.y, forward.z);
    local up = lToW * luajogo.vec4(0, 1, 0, 1);
    up = luajogo.vec3(up.x, up.y, up.z);
    local eye = lToW * luajogo.vec4(0, 0, 0, 1);
    eye = luajogo.vec3(eye.x, eye.y, eye.z);

    local viewMat = luajogo.mat4x4();
    viewMat:lookAt(eye, forward, up - eye);
    local wToVP = projMat * viewMat;

    return wToVP;
end

function setup()

    local scene = {};

    local texAsset = luajogo.assets.load("/tileset.png");
    local tex = luajogo.texture.load(texAsset);

    local fontAsset = luajogo.assets.load("/arial.ttf");
    local font = luajogo.font.load(fontAsset);

    local w = luajogo.window.create(WIDTH, HEIGHT, "luajogo example");
    if not w:isValid() then
        error("Window is invalid");
    end

    local g = luajogo.graphicsContext();
    w:setGraphicsContext(g);

    local vertexShader = g:createShader(VERTEX_CODE, luajogo.VERTEX_SHADER);
    local fragmentShader = g:createShader(FRAGMENT_CODE, luajogo.FRAGMENT_SHADER);
    local fragmentFontShader = g:createShader(FRAGMENT_FONT_CODE, luajogo.FRAGMENT_SHADER);
    local program = g:createShaderProgram(vertexShader, fragmentShader);
    local fontProgram = g:createShaderProgram(vertexShader, fragmentFontShader);

    local texture = g:createTexture(tex);
    local fontTexture = g:createTexture(font:texture());

    local fontMesh = font:makeVertexArray("Hello World!");
    local fontVA = g:createVertexArray(fontMesh.vbuf, fontMesh.ebuf);
    fontVA:setAttribute(0, 3);
    fontVA:setAttribute(1, 2);

    local box = g:createVertexArray(BOX.vbuf, BOX.ebuf);
    box:setAttribute(0, 3);
    box:setAttribute(1, 2);

    local quad = g:createVertexArray(QUAD.vbuf, QUAD.ebuf);
    quad:setAttribute(0, 3);
    quad:setAttribute(1, 2);

    g:setShaderParam("WorldToViewportMatrix", makeWorldToVPMat(luajogo.vec3(0, 0, 2), luajogo.vec3(0, 0, 0), WIDTH, HEIGHT, FOV));
    g:setShaderParam("BaseColor", luajogo.color("white"));

    local gObj = g:createObject();
    gObj:setShaderProgram(program);
    gObj:setTexture(texture);
    gObj:setVertexArray(box);

    local textObj = g:createObject();
    textObj:setShaderProgram(fontProgram);
    textObj:setTexture(fontTexture);
    textObj:setVertexArray(fontVA);

    w:setClearColor(luajogo.color(150, 150, 150, 255));
    g:refreshDrawOrder();

    local lToWMat = luajogo.mat4x4();
    gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

    local textLToW = luajogo.mat4x4();
    textLToW:rotateEuler(luajogo.vec3(180, 0, 0));
    textLToW:scale(luajogo.vec3(1, 1, 1) * 0.01);
    textObj:setShaderParam("LocalToWorldMatrix", textLToW);

    w:setClearColor(luajogo.color(150, 150, 150, 255));
    g:refreshDrawOrder();

    scene.graphics = g;
    scene.gObj = gObj;
    scene.textObj = textObj;

    return {
        window = w,
        graphics = g,
        gObj = gObj,
        textObj = textObj,
    };
end

function onWindowResize(self, data)

end

function onKeyDown(self, data)

end

function onKeyUp(self, data)

end

function onMouseDown(self, data)

end

function onMouseUp(self, data)

end

function onMouseMove(self, data)

end

function main()

    luajogo.assets.configure({
        assetSources = {
            {type = "folder", path = "../assets"}
        }    
    });

    local scene = setup();

    scene.window:addEventCallback("onWindowResize", onWindowResize);
    scene.window:addEventCallback("onKeyDown", onKeyDown);
    scene.window:addEventCallback("onKeyUp", onKeyUp);
    scene.window:addEventCallback("onMouseDown", onMouseDown);
    scene.window:addEventCallback("onMouseUp", onMouseUp);
    scene.window:addEventCallback("onMouseMove", onMouseMove);

    scene.window:open();

    while not scene.window:shouldClose() do
        scene.window:pollEvents();

        scene.window:draw();
    end

end