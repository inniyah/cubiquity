function importVxl()
	volumeWidth = 512
	volumeHeight = 64
	volumeDepth = 512

	result = GameplayColouredCubesVolume.create(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1, 64, 64)

	local colour = Vector4.new(0.9, 0.9, 0.9, 1.0)
	--local empty = Vector4.new(0.0, 0.0, 0.0, 0.0)
	--for iZ=0, volumeWidth - 1 do
	--	for iY=0, 1 do
	--		for iX=0, volumeDepth - 1 do
	--			result:setVoxel(iX, iY, iZ, colour, false)
	--		end
	--	end
	--end
	result:markRegionAsModified(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1)



	local inputFile = io.open("VxlEditor\\ToLoad.vxl", "rb")
	if(inputFile) then
		print("File opened")
	else
		print("Open failed")
	end

	local data = inputFile:read("*all")

	i = 1
	x = 0
	y = 0
	columnI = 0
	mapSize = 512
	columnCount = mapSize * mapSize
	while columnI < columnCount do
		N = data:byte(i)
		S = data:byte(i + 1)
		E = data:byte(i + 2)
		A = data:byte(i + 3)
		K = E - S + 1
		if (N == 0) then
			Z = 0
			M = 64;
		else
			Z = (N-1) - K
			-- A of the next span
			M = data:byte(i + N * 4 + 3)
		end
		colorI = 0
		for p = 0, 2 do
			-- BEWARE: COORDINATE SYSTEM TRANSFORMATIONS MAY BE NEEDED
			-- Get top run of colors
			if (p == 0) then
				zz = S
				runlength = K
			else
				-- Get bottom run of colors
				zz = M - Z
				runlength = Z
			end
			for j = 0, runlength do
				red = data:byte(i + 6 + colorI * 4)
				green = data:byte(i + 5 + colorI * 4)
				blue = data:byte(i + 4 + colorI * 4)
				-- Do something with these colors
				-- makeVoxelColorful(x, y, zz, red, green, blue);
				--local myColour = Vector4.new(red / 255.0, green / 255.0, blue / 255.0, 1.0)
				result:setVoxel(x, zz-1, y, colour, false)
				zz = zz + 1
				colorI = colorI + 1
			end
		end

		-- Now deal with solid non-surface voxels
		-- No color data is provided for non-surface voxels
		zz = E + 1
		runlength = M - Z - zz
		for j = 0, runlength do
			--makeVoxelSolid(x, y, zz);
			result:setVoxel(x, zz-1, y, colour, false)
			zz = zz + 1
		end
		if (N == 0) then
			columnI = columnI + 1
			x = x + 1
			if (x >= mapSize) then
				print(columnI)
				x = 0;
				y = y + 1
				
				print(y)
			end
			i = i + (4*(1 + K))
		else
			i = i + (N * 4)
		end
	end

	return result
end
