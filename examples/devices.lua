--
--  This file is part of the Naphex project
--  Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
--
--  This program is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- Test with on and off devices

print("con1");
dev = ondevice.new("con1");
dev:open("eth0");

print("con2");
dev2 = ondevice.new("con2");
dev2:open("eth0");

print("con3");
fdev = offdevice.new("con3");
fdev:open("../examples/test1.pcap");

print("con4");
fdev2 = offdevice.new("con4");
fdev2:open("../examples/test1.pcap");
