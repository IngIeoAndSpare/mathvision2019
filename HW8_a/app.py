import os
import numpy as np
from numpy import linalg as la
import matplotlib.pylab as plt

## ================ file hander method ====================
def changeFilePath(file_path):
    os.chdir(file_path)

def readFile(file_name) :
    file_data = []
    file_object = open(file_name, 'r')
    while True:
        line_value = file_object.readline()
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

def convertArrayToNpArray(array):
    return np.asanyarray(array)

def getMean(data_list):
    mean_vals = np.mean(data_list, axis=0)
    mean_removed = data_list - mean_vals
    return {
        'mean' : mean_vals,
        'remove_mean' : mean_removed
    }

def getEigPairs(cov_mat):
    eig_values, eig_vector = la.eig(cov_mat)
    eig_values_len = len(eig_values)
    # print(eig_vector)
    # print(eig_values)
    eig_pairs = [
        (np.abs(eig_values[vender_num]), eig_vector[:,vender_num])
        for vender_num in range(eig_values_len)
    ]
    
    # print(eig_pairs) 
    
    eig_pairs.sort(reverse=True)
    return eig_pairs

def getProjection(eig_pairs, data_list, vender_num):
    col_stack_mat = np.hstack((
        eig_pairs[0][1].reshape(vender_num,1),
        eig_pairs[1][1].reshape(vender_num,1)
    ))
    data_array = convertArrayToNpArray(data_list)
    pca = data_array.T.dot(col_stack_mat)
    return pca

def getRankVender(data_list):
    order = np.array(data_list).argsort()
    ranks = order.argsort()
    return ranks

def calculateHandler(data_list_a, data_list_b, vender_num):
    
    total_data_list = data_list_a + data_list_b
    ## vender array set
    ## 0 : sugar_vender || 1 : density_vender || 2 : color_vender || 3 : moisture_vender
    convert_array = [
        getCalArray(total_data_list, number)
        for number in range(vender_num)
    ]
    ## get cov value
    cov_mat = getCovarianceValue(convert_array)
    mean_result = getMean(convert_array)
    eig_pairs = getEigPairs(cov_mat)
    pca = getProjection(eig_pairs, convert_array, vender_num)

    return pca

## ================ cov, rank calculate hander end =====
## ================ draw hander ========================
def platDrawHandler(pca, data_a_length, data_b_length):
    plt.axis('equal')
    for count, pca in enumerate(pca):
        if count < data_a_length :
            plt.scatter(pca[0], pca[1], c='red', alpha=0.7)
        else :
            plt.scatter(pca[0], pca[1], c='blue', alpha=0.7)
          
    plt.show()

def gaussianDrawHandler(pca, data_a_length):
    # TODO : gaussianDraw function
    test = 1


## ================ draw hander end =====================

if __name__ == "__main__":
    file_path = r"E:\Project\gitRepo\mathvision2019\HW8_a"
    file_data_a = readFileHandler("data_a.txt", file_path)
    file_data_b = readFileHandler("data_b.txt", file_path)
    vender_number = 4

    pca_result = calculateHandler(file_data_a, file_data_b, vender_number)
    platDrawHandler(pca_result, len(file_data_a), len(file_data_b))