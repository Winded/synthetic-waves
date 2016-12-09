-- Window test

local w = window.create(640, 480, "My title");

print(w);

while not w:shouldClose() do
    w:clear(0, 0, 1, 1);
    w:swapBuffers();
    w:pollEvents();
end

w:destroy();