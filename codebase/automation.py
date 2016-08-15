nodes = [114, 713, 907, 914, 1408, 1904, 2007, 2014]

rcvd_rssi = [[0, -44.81545617, -51.16082144, -48.38512991, -55.96873822, -60.70291815, -60.48418815, -59.65503833],
             [0, 0, -43.69643288, -31.77698037, -49.3309061, -56.57516808, -55.96873822, -54.74916412],
             [0, 0, 0, -46.6453715, -42.51703052, -51.85382777, -52.53421086, -54.74916412],
             [0, 0, 0, 0, -48.07237783, -55.80788024, -54.74916412, -52.53421086],
             [0, 0, 0, 0, 0, -45.48418815, -44.81545617, -49.15241775],
             [0, 0, 0, 0, 0, 0, -36.2924303, -51.35725091],
             [0, 0, 0, 0, 0, 0, 0, -46.6453715],
             [0, 0, 0, 0, 0, 0, 0, 0]]


pow_mw = [[0 for x in range(8)] for y in range(8)] 
for i in range(8):
	for j in range(8):
		if(rcvd_rssi[i][j] == 0):
			pow_mw[i][j] = 0
			continue
		pow_mw[i][j] = 1000 * pow(10,(rcvd_rssi[i][j]/10))

pow_mw_new = [[0 for x in range(8)] for y in range(8)]
for i in range(8):
	for j in range(8):
		if(i > j):
			pow_mw_new[i][j] = pow_mw[j][i]
			continue
		pow_mw_new[i][j] = pow_mw[i][j]

pow_mw_norm = [[0 for x in range(8)] for y in range(8)]
for i in range(8):
	max_val = max(pow_mw_new[i])
	for j in range(8):
		pow_mw_norm[i][j] = pow_mw_new[i][j]/max_val

#print pow_mw_norm

channel_ini = [6, 1, 1, 1, 1, 1, 1, 11]
ch_1 = []
ch_6 = []
ch_11 = []
sum_array = []
#ch_out

channel_hsum = list(channel_ini)
for a in range(10):
	for i in range(8):
		for j in range(8):
			if(channel_hsum[j] == 1):
				ch_1.append(pow_mw_norm[i][j])
			elif(channel_hsum[j] == 6):
				ch_6.append(pow_mw_norm[i][j])
			elif(channel_hsum[j] == 11):
				ch_11.append(pow_mw_norm[i][j])
		sum_array.append(sum(ch_1))
		sum_array.append(sum(ch_6))
		sum_array.append(sum(ch_11))
		index = sum_array.index(min(sum_array))
		if(index == 0): 
			channel_hsum[i] = 1
		elif(index == 1):
			channel_hsum[i] = 6
		elif(index == 2):
			channel_hsum[i] = 11
		del ch_1[:]
		del ch_6[:]
		del ch_11[:]
		del sum_array[:]
print channel_hsum


channel_hminmax = list(channel_ini)
for a in range(10):
        for i in range(8):
                for j in range(8):
                        if(channel_hminmax[j] == 1):
                                ch_1.append(pow_mw_norm[i][j])
                        elif(channel_hminmax[j] == 6):
                                ch_6.append(pow_mw_norm[i][j])
                        elif(channel_hminmax[j] == 11):
                                ch_11.append(pow_mw_norm[i][j])
                sum_array.append(max(ch_1))
                sum_array.append(max(ch_6))
                sum_array.append(max(ch_11))
                index = sum_array.index(min(sum_array))
                if(index == 0):
                        channel_hminmax[i] = 1
                elif(index == 1):
                        channel_hminmax[i] = 6
                elif(index == 2):
                        channel_hminmax[i] = 11
                del ch_1[:]
                del ch_6[:]
                del ch_11[:]
                del sum_array[:]
print channel_hminmax
