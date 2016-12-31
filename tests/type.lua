local T = {};

function T.__type(self)
	return "TestType";
end

local v = {};
setmetatable(v, T);
print("Type of v = " .. type(v));