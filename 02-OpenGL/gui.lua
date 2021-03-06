startMenu =     { -1.0, 1.0, 1.0, -1.0, 0, 0, 1, 1, 0,         --Background
			      -0.95, -0.497, -0.61, -0.924, 0, 0, 1, 1, 1, --New Game
			      -0.56, -0.497, -0.22, -0.924, 0, 0, 1, 1, 2, --Continue
			      -0.17, -0.497,  0.17, -0.924, 0, 0, 1, 1, 3, --How to play
			       0.22, -0.497,  0.56, -0.924, 0, 0, 1, 1, 4, --Map creation
			       0.61, -0.497,  0.95, -0.924, 0, 0, 1, 1, 5 } -- Exit


howToPlayMenu = { -1.0, 1.0, 1.0, -1.0, 0, 0, 1, 1, 6,      --How to play text
				  -0.841, -0.475, -0.441, -0.875, 0, 0, 1, 1, 7 } --Back

				  
pauseMenu =		{ -0.312, 0.50, 0.312,  0.25 , 0, 0, 1, 1, 14, --Pause
				  -0.312, 0.25, 0.312,  0.0 , 0, 0, 1, 1, 8,  --Resume
				  -0.312, 0.0 , 0.312, -0.25, 0, 0, 1, 1, 9 } --Quit

won = { -0.312, 0.400, 0.312, 0.150, 0, 0, 1, 1, 10, -- Win 
	    -0.312, 0.125, 0.312, -0.125, 0, 0, 1, 1, 9 }  --Quit

defeat = { -0.312, 0.675, 0.312, 0.425, 0, 0, 1, 1, 11,  -- Defeat
		   -0.312, 0.400, 0.312, 0.150, 0, 0, 1, 1, 13,  -- Restart 5G
		   -0.312, 0.125, 0.312, -0.125, 0, 0, 1, 1, 12,  -- Free Restart
		   -0.312, -0.150, 0.312, -0.400, 0, 0, 1, 1, 9 } --Quit




stadardInGameGui = { -0.552, 0.550, 0.552, 0.350, 0, 0, 1, 1, 19, --"You are the victor" text
					 -1.0, 1.0, 1.0, 0.848, 0, 0, 1, 1, 0,          --Background

					 -0.313, 0.987, 0.0  , 0.861, 0, 0, 1, 1, 5, --Sword
					 -0.0  , 0.987, 0.313, 0.861, 0, 0, 1, 1, 6, --Spear 

					-- 0.825, 0.973, 0.950, 0.751,   0, 0, 1, 1, 8, --Combo

				     -0.950, 0.979, -0.8875, 0.869, 0, 0, 1, 1, 1, --Heart
				     -0.8795, 0.979, -0.817, 0.869, 0, 0, 1, 1, 1, --Heart
				     -0.809, 0.979, -0.7465, 0.869, 0, 0, 1, 1, 1 } --Heart
					 
noSavedGame = 0 

function init() --[[ When the game starts, lua returns how the start menu should look like ]]
	return startMenu, 1
end

function clicked( xPos, yPos, state ) --[[ When the player clicks somewhere in a menu state ]]
	if state == 1 then  --[[ Start menu ]]
		if xPos < -0.61 and xPos > -0.95  and yPos < -0.497 and yPos > -0.924 then
			stateChange(1) --[[ New game ]]
		elseif xPos < -0.22 and xPos > -0.56  and yPos < -0.497 and yPos > -0.924 and tonumber(noSavedGame) == 1 then
			stateChange(4) --[[ Continue game ]]
		elseif xPos < 0.17 and xPos > -0.17 and yPos < -0.497 and yPos > -0.924 then
			return howToPlayMenu, 2
		 elseif xPos < 0.56 and xPos > 0.22  and yPos < -0.497 and yPos > -0.924 then
			stateChange(2) --[[ Map Creation ]]
		elseif xPos < 0.95 and xPos > 0.61 and yPos < -0.497 and yPos > -0.924 then
			stateChange(3) --[[ Exit game ]]
		else
			return -1 --[[ Empty space clicked on ]]
		end
	elseif state == 2 then --[[ Getting back to the startMenu from "howToPlayMenu" ]]
		if xPos < -0.441 and xPos > -0.841 and yPos < -0.475 and yPos > -0.875 then --[[Back]]
			return startMenu, 1
		end
	elseif state == 3 then --[[Pause state]]
		if xPos < 0.312 and xPos > -0.312  and yPos < 0.25 and yPos > 0.0 then          --[[Resume]]
			stateChange(0)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < 0.0 and yPos > -0.25 then     --[[Exit to start menu]]
			stateChange(2)
		end
	elseif state == 4 then  --[[Win state]]
		if xPos < 0.312 and xPos > -0.312  and yPos < 0.125 and yPos > -0.125 then --[[Exit to start menu]]
			stateChange(2)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < 0.450 and yPos > 0.150 then --[[Resume / Hidden Button]]
			stateChange(0)
		end
	elseif state == 5 then  --[[Defeat state]]
		if xPos < 0.312 and xPos > -0.312  and yPos < 0.400 and yPos > 0.150 then -- Restart 5G
			stateChange(5)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < 0.125 and yPos > -0.125 then -- Free Restart
			stateChange(6)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < 0.675 and yPos > 0.425 then --[[Resume / Hidden Button]]
			stateChange(0)
		elseif xPos < 0.312 and xPos > -0.312  and yPos < -0.15 and yPos > -0.40 then --[[Exit to start menu]]
			stateChange(2)
		end
	else
		return -1
	end
	
	return -1
end



function pauseGame()
	return pauseMenu, 3
end

function inGameGUI()
	return stadardInGameGui
end

function onVictory()
	return won, 4
end

function onDeath()
	return defeat, 5
end


-----------------------------------------------------------------------Health
heartOne = { -0.7382, 0.979, -0.6757, 0.869, 0, 0, 1, 1, 1 }    --Heart
heartTwo = { -0.6674, 0.979, -0.6049, 0.869, 0, 0, 1, 1, 1 }    --Heart
heartThree = { -0.5966, 0.979, -0.5341, 0.869, 0, 0, 1, 1, 1 }  --Heart

empty = {}
maxHearts = 6
currentHearts = 3

function addHeart() --Return one of the extra hearts you can obtain. Up too 3 extra hearts
	if currentHearts == 3 then
		currentHearts = currentHearts + 1
		return heartOne
	elseif currentHearts == 4 then
		currentHearts = currentHearts + 1
		return heartTwo
	elseif currentHearts == 5 then
		currentHearts = currentHearts + 1
		return heartThree
	else
		return empty
	end
end
-----------------------------------------------------------------------Health END


---------------------------------------------------------------------------SHOP
shopMenu = { -1.0, 1.0, 1.0, -1.0, 0, 0, 1, 1, 0, --Background
			 -0.885, 0.895, -0.483, 0.553, 0, 0, 1, 1, 1,   --Sword
			 -0.885, 0.533, -0.483, 0.191, 0, 0, 1, 1, 5,   --Spear
			 -0.885, 0.171, -0.483, -0.171, 0, 0, 1, 1, 9,  --Health
			 -0.885, -0.191, -0.483, -0.533, 0, 0, 1, 1, 13, --Armor 
			 -0.885, -0.553, -0.483, -0.895, 0, 0, 1, 1, 27, --Heal

			 0.721, 0.760, 0.745, 0.700, 0, 0, 1, 1, 16, --100 gold
			 0.752, 0.760, 0.776, 0.700, 0, 0, 1, 1, 16, --10 gold
			 0.781, 0.760, 0.808, 0.700, 0, 0, 1, 1, 16, --1 gold

			 -0.200, 1.0, 0.424, 0.750, 0, 0, 1, 1, 26 } --Resume

healingUsedTexture = 28
healingUsed = false

			 --sw1 sw2 sw3  end   sp1 sp2 sp3  end  h1  h2  h3  end   a1  a2  end
upgradeCost = { 15, 25, 35, 9999, 15, 25, 35, 9999, 20, 30, 40, 9999, 30, 40, 9999}

textures = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}

upgradeTo = {1, 5, 9, 13} --These numbers are the texture index for each upgrades texture

function shopInit() --Launch the shop
	return shopMenu
end

function shopClickCheck( xPos, yPos, gold ) --[[Checks if the player clicked on any of the clickable things]]
	if xPos < -0.483 and xPos > -0.885  and yPos < 0.853 and yPos > 0.511 then  --[[Sword Upgrade]]
		return swordUpgrade(gold)
	elseif xPos < -0.483 and xPos > -0.885  and yPos < 0.494 and yPos > 0.153 then --[[Spear Upgrade]]
		return spearUpgrade(gold)
	elseif xPos < -0.483 and xPos > -0.885  and yPos < 0.133 and yPos > -0.211 then --[[Health Upgrade]]
		return healthUpgrade(gold)
	elseif xPos < -0.483 and xPos > -0.885  and yPos < -0.23 and yPos > -0.569 then --[[Armor Upgrade]]
		return armorUpgrade(gold)
	elseif xPos < -0.483 and xPos > -0.885  and yPos < -0.588 and yPos > -0.93 then --[[Heal]]
		return healCheck(gold)
	elseif xPos < 0.424 and xPos > -0.200  and yPos < 1.0 and yPos > 0.750 then     --[[Continue]]
		healingUsed = false
		stateChange(2)	
	end
	return -1, -1, gold
end


function swordUpgrade( gold )
	if upgradeCost[upgradeTo[1]] <= gold then
		gold = gold - upgradeCost[upgradeTo[1]]
		upgradeTo[1] = textures[upgradeTo[1] + 1]
		return textures[upgradeTo[1]], 1, gold
	end
	return -1, -1, gold
end

function spearUpgrade( gold )
	if upgradeCost[upgradeTo[2]] <= gold then
		gold = gold - upgradeCost[upgradeTo[2]]
		upgradeTo[2] = textures[upgradeTo[2] + 1]
		return textures[upgradeTo[2]], 2, gold
	end
	return -1, -1, gold
end

function healthUpgrade( gold )
	if upgradeCost[upgradeTo[3]] <= gold then
		gold = gold - upgradeCost[upgradeTo[3]]
		upgradeTo[3] = textures[upgradeTo[3] + 1]
		currentHearts = currentHearts + 1
		return textures[upgradeTo[3]], 3, gold
	end
	return -1, -1, gold
end

function armorUpgrade( gold )
	if upgradeCost[upgradeTo[4]] <= gold then
		gold = gold - upgradeCost[upgradeTo[4]]
		upgradeTo[4] = textures[upgradeTo[4] + 1]
		return textures[upgradeTo[4]], 4, gold
	end
	return -1, -1, gold
end

function healCheck(gold)
	if gold >= 5 and healingUsed ~= true then
		gold = gold - 5
		healingUsed = true
	end
	return healingUsedTexture, 5, gold;
end

function setHealingUsed()
	healingUsed = true
end
---------------------------------------------------------------------------SHOP END

---------------------------------------------------------------------------Save/Load

function saveGame(gold, whichMap) --[[Saves the game on each new map, doesn't save which wave the player was on]]
	local file = io.open("savedGame.dat", "w+")	-- Opens a file in read
	io.output(file)							-- sets the default input file as test.lua

	io.write(currentHearts - 3, "\n")
	io.write(upgradeTo[1] - 1, "\n")
	io.write(upgradeTo[2] - 5, "\n")
	io.write(upgradeTo[3] - 9, "\n")
	io.write(upgradeTo[4] - 13, "\n")
	io.write(whichMap, "\n")
	io.write(gold, "\n")

	io.close(file)
end

function onDefeatSave(gold, whichMap, whichWave, nrOfExtraHealth) --[[Saves the game when the player dies, saves the which wave the player was on too]]
	local file = io.open("defeatSave.dat", "w+")	-- Opens a file in read
	io.output(file)							-- sets the default input file as test.lua

	io.write(nrOfExtraHealth - 3, "\n")
	io.write(upgradeTo[1] - 1, "\n")
	io.write(upgradeTo[2] - 5, "\n")
	io.write(upgradeTo[3] - 9, "\n")
	io.write(upgradeTo[4] - 13, "\n")
	io.write(whichMap, "\n")


	io.write(whichWave, "\n")
	io.write(gold, "\n")

	io.close(file)
end

function onDefeatLoad(gameUI, shopUI, player, map, wave) --[[Loads the save that was created upon death]]
	local counter = 0
	local tmp = {}
	local file = io.open("defeatSave.dat", "r")
	io.input(file)

	while counter < 9
	do
		counter = counter + 1
		tmp[counter] = io.read("*l")
	end
	counter = nil

	upgradeTo[1] = tmp[2] + 1
	upgradeTo[2] = tmp[3] + 5
	upgradeTo[3] = tmp[4] + 9
	upgradeTo[4] = tmp[5] + 13
	
	io.close(file)

	savedGameInfo(tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[8], true, wave, map, shopUI, gameUI, player)

end

function loadGame(gameUI, shopUI, player, map, wave) --[[Loads the save that was created on the last map which]]
	local counter = 0
	local tmp = {}
	local file = io.open("savedGame.dat", "r")
	io.input(file)

	while counter < 8
	do
		counter = counter + 1
		tmp[counter] = io.read("*l")
	end
	counter = nil

	upgradeTo[1] = tmp[2] + 1
	upgradeTo[2] = tmp[3] + 5
	upgradeTo[3] = tmp[4] + 9
	upgradeTo[4] = tmp[5] + 13
	
	io.close(file)

	savedGameInfo(tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], false, wave, map, shopUI, gameUI, player)

end

function checkSavedGameFile()
	local file = io.open("savedGameExist.dat", "r")
	io.input(file)

	noSavedGame = io.read("*l")

	io.close(file)

	return noSavedGame
end
---------------------------------------------------------------------------Save/Load END
