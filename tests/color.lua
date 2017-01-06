print("-----");
print("COLOR TEST");

local c = luajogo.color(255, 255, 255, 255);
print("Color = " .. tostring(c));
print("Color R = " .. c.r);

print("White = " .. tostring(luajogo.color("white")));
print("Red = " .. tostring(luajogo.color("red")));
print("Green = " .. tostring(luajogo.color("green")));
print("Blue = " .. tostring(luajogo.color("blue")));

print("-----");