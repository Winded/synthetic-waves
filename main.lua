-- Test script

local w = window(640, 480, "My title");
local g = graphicsContext();
w:setGraphicsContext(g);

local vertexShader = g:createVertexShader([[
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
]]);
local fragmentShader = g:createFragmentShader([[
#version 330

in vec2 UV;

out vec4 outColor;

uniform sampler2D Texture1;
uniform vec4 BaseColor;

void main()
{
    outColor = texture2D(Texture1, UV) * BaseColor;
}
]]);
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

g:setShaderParam("WorldToViewportMatrix", mat4x4());
g:setShaderParam("BaseColor", color("white"));

local gObj = g:createObject();
gObj:setShaderProgram(program);
gObj:setTexture(texture);
gObj:setVertexArray(vertexArray);

local lToWMat = mat4x4();
gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

w:setClearColor(color(150, 150, 150, 255));

while not w:shouldClose() do
    w:pollEvents();

    local delta = w:deltaTime();
    lToWMat:rotateEuler(vec3(0, 0, 45 * DEG2RAD * delta));
    gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

    w:draw();
end

w:destroy();
