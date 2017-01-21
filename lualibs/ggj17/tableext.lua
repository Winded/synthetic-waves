
function table.contains(table, element)
    for _, value in pairs(table) do
        if value == element then
            return true;
        end
    end
    return false;
end

function table.removeValue(table, value)
    for idx, val in pairs(table) do
        if value == val then
            table.remove(table, idx);
            return;
        end
    end
end