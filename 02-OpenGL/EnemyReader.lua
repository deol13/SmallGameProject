function errhandler(errmsg) 
	return errmsg .. "\n" .. debug.traceback()
end
	
function readFile()
	enemyArr = {}
	texArr = {}
	texCount = 0;
	file = io.open("EnemyWaves.dat", "r")	-- Opens a file in read
	io.input(file)							-- sets the default input file as test.lua

	if waveNr == nil then waveNr = 1 end
	
	tempTester = "x" .. waveNr
	temp = io.read("*l")
	
	while temp ~= tempTester				--Finds the correct wave that we are searching for
	do
		temp = io.read("*l")
	end

	nrOfEnemies = io.read("*l")				-- prints the first line of the file then moves the pointer to the next line 
	nrOfEnemies = tonumber(nrOfEnemies)
	if nrOfEnemies ~= nil or nrOfEnemies ~= 0 --Is there any meaning to doing the rest?
	then
		c = nrOfEnemies * 6	+ 1
		i = 1
		while i < c
		do
			if i % 6 == 1								-- decide Ai type
			then
				enemyType = io.read("*l")	
				if enemyType == "melee"			
				then
					enemyArr[i] = 1
				else
					enemyArr[i] = 0
				end
				i = i + 1
			elseif i % 6 == 3							-- decide texture index, while avoiding repeats
			then
				enemyArr[i] = io.read("*l")
				enemyArr[i + 1] = findTexture(enemyArr[i])
				i = i + 2--]]
			else
					enemyArr[i] = io.read("*l")
					i = i + 1
			end			
		end
	end		
	
	io.close(file)							-- closes the open file

	return enemyArr, nrOfEnemies
end


function findTexture(name)			-- Matches the texture names to their GLuint. 
	--check for new textures
	if texArr[name] == nil then
		texCount = texCount + 1
		texArr[name] = texCount
	end
	return texArr[name]							--return the texture matching the file name
end