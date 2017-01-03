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

local BOX = {
    vbuf = {
        -0.5, -0.5, 0.5, 0, 0,
        -0.5, -0.5, -0.5, 0, 0,
        -0.5, 0.5, -0.5, 0, 0,
        -0.5, 0.5, 0.5, 0, 0,
        0.5, -0.5, 0.5, 0, 0,
        0.5, -0.5, -0.5, 0, 0,
        0.5, 0.5, -0.5, 0, 0,
        0.5, 0.5, 0.5, 0, 0,
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

local w = window(WIDTH, HEIGHT, "luajogo example");
local g = graphicsContext();
w:setGraphicsContext(g);

local vertexShader = g:createShader(VERTEX_CODE, VERTEX_SHADER);
local fragmentShader = g:createShader(FRAGMENT_CODE, FRAGMENT_SHADER);
local program = g:createShaderProgram(vertexShader, fragmentShader);

local texture = g:createTexture({255, 255, 255, 255}, 1, 1);

local box = g:createVertexArray(BOX.vbuf, BOX.ebuf);
box:setAttribute(0, 3);
box:setAttribute(1, 2);

local quad = g:createVertexArray(QUAD.vbuf, QUAD.ebuf);
quad:setAttribute(0, 3);
quad:setAttribute(1, 2);

local camMat = mat4x4();
camMat:translateInPlace(vec3(0, 0, 2));
local camMovement = vec3();
local camRotation = vec3();
local function updateWorldToVPMat()
    local projMat = mat4x4();
    local orthoScale = 5;
    local aspectRatio = WIDTH / HEIGHT;
    local orthoWidth = orthoScale * aspectRatio;
    local orthoHeight = orthoScale;
    projMat:perspective(FOV, aspectRatio, 0, 100);
    local lToW = camMat;
    local forward = lToW * vec4(0, 0, -1, 1);
    forward = vec3(forward.x, forward.y, forward.z);
    local up = lToW * vec4(0, 1, 0, 1);
    up = vec3(up.x, up.y, up.z);
    local camPos = camMat * vec4(0, 0, 0, 1);
    camPos = vec3(camPos.x, camPos.y, camPos.z);
    local viewMat = mat4x4();
    viewMat:lookAt(camPos, forward, up - camPos);
    local wToVP = projMat * viewMat;
    g:setShaderParam("WorldToViewportMatrix", wToVP);
end

updateWorldToVPMat();

g:setShaderParam("BaseColor", color("red"));

local gObj = g:createObject();
gObj:setShaderProgram(program);
gObj:setTexture(texture);
--gObj:setVertexArray(quad);
gObj:setVertexArray(box);

w:setClearColor(color(150, 150, 150, 255));
g:refreshDrawOrder();

local lToWMat = mat4x4();
gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

local rotateBox = false;
local rotateDir = vec3(1, 1, 1);

w:addEventCallback("onWindowResize", function(self, data)
    WIDTH = data.width;
    HEIGHT = data.height;
    updateWorldToVPMat();
end);

w:addEventCallback("onKeyDown", function(self, data)
    local k = data.key:lower();
    if k == "w" then
        camMovement = vec3(camMovement.x, camMovement.y, -1);
    elseif k == "s" then
        camMovement = vec3(camMovement.x, camMovement.y, 1);
    elseif k == "a" then
        camMovement = vec3(-1, camMovement.y, camMovement.z);
    elseif k == "d" then
        camMovement = vec3(1, camMovement.y, camMovement.z);
    elseif k == "q" then
        camRotation = vec3(camRotation.x, -45, camRotation.z);
    elseif k == "e" then
        camRotation = vec3(camRotation.x, 45, camRotation.z);
    elseif k == "z" then
        camRotation = vec3(-45, camRotation.y, camRotation.z);
    elseif k == "x" then
        camRotation = vec3(45, camRotation.y, camRotation.z);
    elseif k == "space" then
        rotateBox = not rotateBox;
    end
    updateWorldToVPMat();
end);

w:addEventCallback("onKeyUp", function(self, data)
    local k = data.key:lower();
    if k == "w" or k == "s" then
        camMovement = vec3(camMovement.x, camMovement.y, 0);
    elseif k == "a" or k == "d" then
        camMovement = vec3(0, camMovement.y, camMovement.z);
    elseif k == "q" or k == "e" then
        camRotation = vec3(camRotation.x, 0, camRotation.z);
    elseif k == "z" or k == "x" then
        camRotation = vec3(0, camRotation.y, camRotation.z);
    end
end);

w:open();

while not w:shouldClose() do
    w:pollEvents();

    if camMovement ~= vec3() or camRotation ~= vec3() then
        camMat:translateInPlace(camMovement * w:deltaTime());
        camMat:rotateEuler(camRotation * w:deltaTime());
        updateWorldToVPMat();
    end

    if rotateBox then
        local r = 45 * w:deltaTime();
        lToWMat:rotateEuler(rotateDir);
        gObj:setShaderParam("LocalToWorldMatrix", lToWMat);
    end

    w:draw();
end