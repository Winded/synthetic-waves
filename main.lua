-- Test script

local WIDTH = 640;
local HEIGHT = 480;

local w = window(WIDTH, HEIGHT, "My title");
local g = graphicsContext();
w:setGraphicsContext(g);

local vertexShader = g:createShader([[
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
]], VERTEX_SHADER);
local fragmentShader = g:createShader([[
#version 330

in vec2 UV;

out vec4 outColor;

uniform sampler2D Texture1;
uniform vec4 BaseColor;

void main()
{
    outColor = texture2D(Texture1, UV) * BaseColor;
}
]], FRAGMENT_SHADER);
local program = g:createShaderProgram(vertexShader, fragmentShader);

local texture = g:createTexture({255, 255, 255, 255}, 1, 1);

local box = g:createVertexArray({
    -0.5, -0.5, 0.5, 0, 0,
    -0.5, -0.5, -0.5, 0, 0,
    -0.5, 0.5, -0.5, 0, 0,
    -0.5, 0.5, 0.5, 0, 0,
    0.5, -0.5, 0.5, 0, 0,
    0.5, -0.5, -0.5, 0, 0,
    0.5, 0.5, -0.5, 0, 0,
    0.5, 0.5, 0.5, 0, 0,
}, {
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
});
box:setAttribute(0, 3);
box:setAttribute(1, 2);

local quad = g:createVertexArray({
    -0.5, 0.5, 0, 0, 0,
    0.5, 0.5, 0, 0, 1,
    0.5, -0.5, 0, 1, 1,
    -0.5, -0.5, 0, 1, 0
}, {
    0, 1, 3,
    1, 2, 3
});
quad:setAttribute(0, 3);
quad:setAttribute(1, 2);

local camPos = vec3(0, 0, 2);
local camRot = vec3(0, 0, 0);
local function updateWorldToVPMat()
    local projMat = mat4x4();
    local orthoScale = 5;
    local aspectRatio = WIDTH / HEIGHT;
    local orthoWidth = orthoScale * aspectRatio;
    local orthoHeight = orthoScale;
    --projMat:ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, 0, 100);
    projMat:perspective(60, aspectRatio, 1, 100);
    local wToL = mat4x4();
    wToL:translate(camPos);
    wToL:rotateEuler(camRot);
    wToL:invert();
    local wToVP = projMat * wToL;
    g:setShaderParam("WorldToViewportMatrix", wToVP);
end

updateWorldToVPMat();

g:setShaderParam("BaseColor", color("red"));

local gObj = g:createObject();
gObj:setShaderProgram(program);
gObj:setTexture(texture);
--gObj:setVertexArray(quad);
gObj:setVertexArray(box);

local lToWMat = mat4x4();
gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

w:setClearColor(color(150, 150, 150, 255));
g:refreshDrawOrder();

w:addEventCallback("onWindowResize", function(self, data)
    WIDTH = data.width;
    HEIGHT = data.height;
    updateWorldToVPMat();
end);

w:addEventCallback("onKeyDown", function(self, data)
    if data.key == "W" then
        camPos = camPos + vec3(0, 0, -0.1);
    elseif data.key == "S" then
        camPos = camPos + vec3(0, 0, 0.1);
    elseif data.key == "A" then
        camPos = camPos + vec3(-0.1, 0, 0);
    elseif data.key == "D" then
        camPos = camPos + vec3(0.1, 0, 0);
    elseif data.key == "Q" then
        camRot = camRot + vec3(0, -10, 0);
    elseif data.key == "E" then
        camRot = camRot + vec3(0, 10, 0);
    elseif data.key == "Z" then
        camRot = camRot + vec3(10, 0, 0);
    elseif data.key == "X" then
        camRot = camRot + vec3(-10, 0, 0);
    end
    updateWorldToVPMat();
end);

while not w:shouldClose() do
    w:pollEvents();

    local delta = w:deltaTime();
    --lToWMat:rotateEuler(vec3(0, 0, 45 * delta));
    lToWMat:rotateEuler(vec3(0, 45 * delta, 0));
    gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

    w:draw();
end