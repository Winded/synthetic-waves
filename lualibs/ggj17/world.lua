
World = {};

function World.load()
    World._texture = luajogo.texture.load(luajogo.assets.load("/world.png"));

    World._sprite = lj2d.graphics.createSprite(World._texture);
    World._sprite:setPivot(luajogo.vec2(0.5, 0.5));
    local size = World._sprite:size();
    print(size);

    World._aabbs = {};
    table.insert(World._aabbs, AABB.createBox(luajogo.vec2(635 - size.x / 2, (size.y - 522) - size.y / 2), luajogo.vec2(16, 832), LAYER_WORLD));
    table.insert(World._aabbs, AABB.createBox(luajogo.vec2(1443 - size.x / 2, (size.y - 522) - size.y / 2), luajogo.vec2(16, 832), LAYER_WORLD));
    table.insert(World._aabbs, AABB.createBox(luajogo.vec2(635 - size.x / 2, (size.y - 522) - size.y / 2), luajogo.vec2(824, 15), LAYER_WORLD));
    table.insert(World._aabbs, AABB.createBox(luajogo.vec2(635 - size.x / 2, (size.y - 1339) - size.y / 2), luajogo.vec2(824, 15), LAYER_WORLD));
    table.insert(World._aabbs, AABB.createBox(luajogo.vec2(860 - size.x / 2, (size.y - 746) - size.y / 2), luajogo.vec2(65, 65), LAYER_WORLD));

    for k, v in pairs(World._aabbs) do
        print(v:position());
    end
end

function World.draw()
    World._sprite:draw();
end

function World.quit()
    World._sprite:destroy();
    for k, v in pairs(World._aabbs) do
        v:destroy();
    end
end