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

pid = thread.new();
pid:set_function("thread1");
pid:set_file("../tests/thread_f.lua");
pid:run();
os.execute("sleep 2");
pid:detach();
