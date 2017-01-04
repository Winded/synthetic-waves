print("-----");
print("ASSET TEST");

assets.configure({
    assetSources = {
        {type = "folder", path = "../assets"}
    }    
});

for k, v in pairs(assets.listAssets()) do
    print(k .. " = " .. v);
end

print("-----");