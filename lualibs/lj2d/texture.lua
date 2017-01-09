
function lj2d.createTexture(pixels, width, height)
    return luajogo.texture.create(pixels, width, height);
end

function lj2d.loadTexture(path)
    local asset = luajogo.assets.load(path);
    return luajogo.texture.load(asset);
end