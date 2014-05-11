nargs = args.size()
print("Args size: " .. nargs);

for n = 1, nargs do
  print("Argument[" .. n .. "]:" .. args.get(n))
end
