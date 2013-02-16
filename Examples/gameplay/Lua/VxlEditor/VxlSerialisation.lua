function importVxl()
	volumeWidth = 512
	volumeHeight = 64
	volumeDepth = 512

	result = GameplayColouredCubesVolume.create(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1, 64, 64)

	local colour = Vector4.new(0.9, 0.9, 0.9, 1.0)
	local empty = Vector4.new(0.0, 0.0, 0.0, 0.0)
	for iZ=0, volumeWidth - 1 do
		for iY=0, volumeHeight - 1 do
			for iX=0, volumeDepth - 1 do
				if(iY < 10) then
					result:setVoxel(iX, iY, iZ, colour, false)
				--else
					--result:setVoxel(iX, iY, iZ, empty, false)
				end
			end
		end
	end
	result:markRegionAsModified(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1)

	return result
end
