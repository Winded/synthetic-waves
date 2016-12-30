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

local vertexArray = g:createVertexArray({
    -0.5, 0.5, 0, 0, 0,
    0.5, 0.5, 0, 0, 1,
    0.5, -0.5, 0, 1, 1,
    -0.5, -0.5, 0, 1, 0
}, {
    0, 1, 3,
    1, 2, 3
});
vertexArray:setAttribute(0, 3);
vertexArray:setAttribute(1, 2);

local projMat = mat4x4();
local orthoScale = 5;
local aspectRatio = WIDTH / HEIGHT;
local orthoWidth = orthoScale * aspectRatio;
local orthoHeight = orthoScale;
projMat:ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, 0, 100);
local wToVP = mat4x4();
wToVP:translate(vec3(1, 0, 0));
wToVP = projMat * wToVP;
g:setShaderParam("WorldToViewportMatrix", wToVP);
g:setShaderParam("BaseColor", color("red"));

local gObj = g:createObject();
gObj:setShaderProgram(program);
gObj:setTexture(texture);
gObj:setVertexArray(vertexArray);

local lToWMat = mat4x4();
gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

w:setClearColor(color(150, 150, 150, 255));
g:refreshDrawOrder();

while not w:shouldClose() do
    w:pollEvents();

    local delta = w:deltaTime();
    lToWMat:rotateEuler(vec3(0, 0, 45 * delta));
    gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

    w:draw();
end

g:destroy();
w:destroy();
