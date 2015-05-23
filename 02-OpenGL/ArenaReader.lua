function errhandler(errmsg) 
	return errmsg .. "\n" .. debug.traceback()
end
	
function readFile()
	arenaArr = {}
	texArr = {}
	texCount = 0;
	file = io.open(fileName .. ".txt", "r")	-- Opens a file in read
	io.input(file)							-- sets the default input file as test.lua

	nrOfObjects = io.read("*l")				-- prints the first line of the file then moves the pointer to the next line 
	nrOfObjects = tonumber(nrOfObjects)
	if nrOfObjects ~= nil or nrOfObjects ~= 0 --Is there any meaning to doing the rest?
	then
		c = nrOfObjects * 8	+ 1
		i = 1
		while i < c
		do
			arenaArr[i] = io.read("*l")	
			if i % 8 == 5
			then
				arenaArr[i + 1] = findTexture(arenaArr[i])
				i = i + 1
			end
			i = i + 1
		end
	end		
	
	io.close(file)							-- closes the open file

	return arenaArr, nrOfObjects
end

function findTexture(name)			-- Matches the texture names to their GLuint. 
	--check for new textures
	if texArr[name] == nil then
		texCount = texCount + 1
		texArr[name] = texCount
	end
	return texArr[name]							--return the texture matching the file name
end
