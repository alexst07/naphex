t = list_all_devs()
for k,v in pairs(t) do
  print(v["name"] .." - " .. v["description"])
  for a,b in pairs(v["ip"]) do
    if b["version"] == 4 then
      if b["addr"] ~= nil then
        print("addr: " .. b["addr"])
      end

      if b["netmask"] ~= nil then
        print("netmask: " .. b["netmask"])
      end

      if b["broadaddr"] ~= nil then
        print("broadaddr: " .. b["broadaddr"])
      end
    end

    if b["version"] == 6 then
      if b["addr"] ~= nil then
        print("addr: " .. b["addr"])
      end
    end
  end
  print("==================")
end