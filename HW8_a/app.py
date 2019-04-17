import os
import numpy as np

## ================ file hander method ====================
def changeFilePath(file_path):
    os.chdir(file_path)

def readFile(fileName) :
    file_data = []
    fileObject = open(fileName, 'r')
    while True:
        line_value = fileObject.readline()
        if not line_value:
            break
        item_array = getStringNumberArray(line_value.strip())
        file_data.append(item_array)

    return file_data

def getStringNumberArray(string_line):
    ## stringLine = '3.0279,2.5626,0.33556,2.4754'
    ## vender -> 당도, 밀도, 색상, 수분함량
    item_array = [float(item) for item in string_line.split(',')]
    return item_array

def readFileHandler(file_name, file_path):
    changeFilePath(file_path)
    file_content = readFile(file_name)

    return file_content
## ================ file hander method end ================
## ================ cov, rank calculate hander method =====
def getCovarianceValue(vender_data):
    cov_val = np.cov(vender_data)
    return cov_val

def getCalArray(data_list, cal_num):
    convert_list = [item[cal_num] for item in data_list]
    return convert_list

def getRankVender(data_list):
    order = np.array(data_list).argsort()
    ranks = order.argsort()

    return ranks

def calculateHandler(data_list_a, data_list_b):
    ## dim num
    total_data_list = data_list_a + data_list_b
    ## vender array set
    sugar_vender = getCalArray(total_data_list, 0)
    density_vender = getCalArray(total_data_list, 1)
    color_vender = getCalArray(total_data_list, 2)
    moisture_vender = getCalArray(total_data_list, 3)
    ## get cov value
    cov_mat = [
        getCovarianceValue(sugar_vender),
        getCovarianceValue(density_vender),
        getCovarianceValue(color_vender),
        getCovarianceValue(moisture_vender)
    ]
    ## get rank 0 = min,  3 = max
    rank_value = getRankVender(cov_mat)
## ================ cov, rank calculate hander end =====
## ================ draw hander ========================



## ================ draw hander end ====================

if __name__ == "__main__":
    file_path = r"E:\Project\gitRepo\mathvision2019\HW8_a"
    file_data_a = readFileHandler("data_a.txt", file_path)
    file_data_b = readFileHandler("data_b.txt", file_path)

    calculateHandler(file_data_a, file_data_b)
