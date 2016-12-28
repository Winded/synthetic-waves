-- Test script

local v = vec3(1, 2, 3);

print("vec3 = " .. tostring(v));
print("vec3 y = " .. v.y);
print("vec3 len = " .. v:length());

print("vec3 + (3, 3, 3) = " .. tostring(v + vec3(3, 3, 3)));
print("vec3 * 2 = " .. tostring(v * 2));
print("vec3 / 2 = " .. tostring(v / 2));
print("-vec3 = " .. tostring(-v));

print("vec3 dot = " .. tostring(v:dotProduct(v)));
print("vec3 cross = " .. tostring(v:crossProduct(v)));

print("vec3 normalized = " .. tostring(v:normalized()));

print("Setting vec3 value");
v.x = 999.995;
print("vec3 = " .. tostring(v));
print("Normalizing...");
v:normalize();
print("vec3 = " .. tostring(v));

print("vec3 type = " .. type(v));

local v2 = vec2(5, 6);
local v3 = vec4(3, 6, 1, 5);
print("vec2 = " .. tostring(v2));
print("vec4 = " .. tostring(v3));
print("vec4 w = " .. tostring(v3.w));

local m = mat4x4();
print(tostring(m));
print("mat4x4 row 1 col 1 = " .. m:get(1, 1));
print("mat4x4 row 1 = " .. tostring(m:row(1)));

-- local w = window.create(640, 480, "My title");
-- local g = graphics.createContext();

-- local vertexShader = g:createVertexShader([[
-- #version 330

-- in vec3 position;
-- in vec2 uvCoordinates;

-- out vec2 UV;

-- uniform mat4 WorldToViewportMatrix;
-- uniform mat4 LocalToWorldMatrix;

-- void main()
-- {
--     gl_Position = WorldToViewportMatrix * LocalToWorldMatrix * vec4(position.xyz, 1.0);
--     UV = uvCoordinates;
-- }
-- ]]);
-- local fragmentShader = g:createFragmentShader([[
-- #version 330

-- in vec2 UV;

-- out vec4 outColor;

-- uniform sampler2D Texture1;
-- uniform vec4 BaseColor;

-- void main()
-- {
--     outColor = texture2D(Texture1, UV) * BaseColor;
-- }
-- ]]);
-- local program = g:createShaderProgram(vertexShader, fragmentShader);

-- local texture = g:createTexture({255, 255, 255, 255}, 1, 1);

-- local vertexArray = g:createVertexArray({
--     -0.5, 0.5, 0, 0, 0,
--     0.5, 0.5, 0, 0, 1,
--     0.5, -0.5, 0, 1, 1,
--     -0.5, -0.5, 0, 1, 0
-- }, {
--     0, 1, 3,
--     1, 2, 3
-- });
-- vertexArray:setAttribute(0, 3);
-- vertexArray:setAttribute(1, 2);

-- g:setShaderParam("WorldToViewportMatrix", mat4x4());
-- g:setShaderParam("BaseColor", vec4(1, 1, 1, 1));

-- local gObj = g:createObject();
-- gObj:setShaderProgram(program);
-- gObj:setTexture(texture);
-- gObj:setVertexArray(vertexArray);

-- local lToWMat = mat4x4();
-- gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

-- while not w:shouldClose() do
--     w:pollEvents();

--     local delta = w:getDeltaTime();
--     lToWMat:rotateZ(45 * DEG2RAD * delta);
--     gObj:setShaderParam("LocalToWorldMatrix", lToWMat);

--     g:clear(0.5, 0.5, 0.5, 1);

--     g:draw();

--     w:swapBuffers();
-- end

-- w:destroy();
