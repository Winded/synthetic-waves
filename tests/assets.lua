print("-----");
print("ASSET TEST");

luajogo.assets.configure({
    assetSources = {
        {type = "folder", path = "../assets"}
    }    
});

for k, v in pairs(luajogo.assets.listAssets()) do
    print(k .. " = " .. v);
end

print("-----");