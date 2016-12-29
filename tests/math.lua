print("-----");
print("PERFORMING MATH TESTS");

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

m:translate(vec3(4, 6, 4));
print("mat4x4 moved by [4, 6, 4]");
print(tostring(m));
m:rotateEuler(vec3(90, 0, 0));
print("mat4x4 rotated by [90, 0, 0]");
print(tostring(m));
m:scale(2);
print("mat4x4 scaled by 2");
print(tostring(m));

print("-----");