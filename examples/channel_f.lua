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

function thread1()
  print("Thread 1 executed from LUA");
  ch1 = channel.get("channel1");
  print("Channel1: " .. ch1);
  os.execute("sleep 5");
  channel.set("channel1", "changed");
  print("Thread 1 finished from LUA");
end

