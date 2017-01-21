
local camera = {};
lj2d.graphics.camera = camera;

camera._lockedToPixels = true;
camera._pos = luajogo.vec2();
camera._rot = 0;
camera._zoom = 1;
camera._viewMat = luajogo.mat4x4();

function camera._hookEvents()
    lj2d.window._window:addEventCallback("onWindowResize", function(w, data)
        camera._refreshViewMat();
    end);
end

function camera._refreshViewMat()
    local windowSize = lj2d.window.size();
    local aspectRatio = windowSize.x / windowSize.y;
    local width = windowSize.y * aspectRatio * camera._zoom;
    local height = windowSize.y * camera._zoom;
    local projMat = luajogo.mat4x4();
    projMat:ortho(-width / 2, width / 2, -height / 2, height / 2, 1, 100);

    local camMat = luajogo.mat4x4();
    if camera._lockedToPixels then
        local pos = windowSize / 2;
        camMat:translateInPlace(luajogo.vec3(pos.x, pos.y, 1));
    else
        camMat:translateInPlace(luajogo.vec3(camera._pos.x, camera._pos.y, 1));
        camMat:rotateEuler(luajogo.vec3(0, 0, camera._rot));
    end
    camMat:invert();

    camera._viewMat = projMat * camMat;
end

function camera.reset()
    camera._lockedToPixels = true;
    camera._pos = luajogo.vec2();
    camera._rot = 0;
    camera._zoom = 1;
    camera._refreshViewMat();
end

function camera.viewMatrix()
    return camera._viewMat;
end

function camera.position()
    return camera._pos;
end

function camera.setPosition(pos)
    camera._pos = pos;
    camera._lockedToPixels = false;
    camera._refreshViewMat();
end

function camera.rotation()
    return camera._rot;
end

function camera.setRotation(rot)
    camera._rot = rot;
    camera._lockedToPixels = false;
    camera._refreshViewMat();
end

function camera.zoom()
    return camera._zoom;
end

function camera.screenToWorldPosition(screenPos)
    local screenRes = lj2d.window.size();
    local screenNorm = luajogo.vec2();
    screenNorm.x = (screenPos.x / screenRes.x) * 2 - 1;
    screenNorm.y = -((screenPos.y / screenRes.y) * 2 - 1);

    local worldPos = camera._viewMat:inverted() * luajogo.vec4(screenNorm.x, screenNorm.y, 0, 1);
    return luajogo.vec2(worldPos.x, worldPos.y);
end

function camera.worldToScreenPosition(worldPos)
    -- todo
end