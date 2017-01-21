
function table.contains(t, element)
    for _, value in pairs(t) do
        if value == element then
            return true;
        end
    end
    return false;
end

function table.removeValue(t, value)
    for idx, val in pairs(t) do
        if value == val then
            table.remove(t, idx);
            return;
        end
    end
end