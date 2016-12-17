-- Test script

local w = window.create(640, 480, "My title");
local g = graphics.createContext();

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

local graphicsObject = g:createObject({
  -0.5, 0.5, 0, 0, 0,
  0.5, 0.5, 0, 0, 1,
  0.5, -0.5, 0, 1, 1,
  -0.5, -0.5, 0, 1, 0
}, {
  0, 1, 3,
  1, 2, 3
});
graphicsObject:setAttribute(0, 3);
graphicsObject:setAttribute(1, 2);

g:setShaderParam("WorldToViewportMatrix", mat4x4());
g:setShaderParam("BaseColor", vec4(1, 1, 1, 1));

local lToWMat = mat4x4();

while not w:shouldClose() do
    w:pollEvents();

    local delta = w:getDeltaTime();
    lToWMat:rotateZ(45 * DEG2RAD * delta);
    g:setShaderParam("LocalToWorldMatrix", lToWMat);

    g:clear(0.5, 0.5, 0.5, 1);

    g:useProgram(program);
    g:useTexture(texture);
    g:useObject(graphicsObject);
    g:useShaderParams();

    g:draw();

    w:swapBuffers();
end

w:destroy();
