import os
import numpy as np
import matplotlib.pylab as plt
from numpy import linalg as la
from mpl_toolkits.mplot3d import Axes3D
from scipy.stats import multivariate_normal
from scipy.spatial.distance import mahalanobis

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
    eig_pairs = getEigPairs(cov_mat)
    pca = getProjection(eig_pairs, convert_array, vender_num)

    return {
        'pca' : pca,
        'eig_pairs' : eig_pairs
    }


def getCalculateGaussian(data_boundary_len, pca):
    pca_a = np.array(pca[:data_boundary_len])
    pca_b = np.array(pca[data_boundary_len:])

    mean_a_array = np.mean(pca_a, axis=0)
    mean_b_array = np.mean(pca_b, axis=0)

    cov_a = getCovarianceValue((pca_a[:, 0], pca_a[:, 1]))
    cov_b = getCovarianceValue((pca_b[:, 0], pca_b[:, 1]))
    grid_x_a, grid_y_a = np.meshgrid(pca_a[:, 0], pca_a[:, 1])
    grid_x_b, grid_y_b = np.meshgrid(pca_b[:, 0], pca_b[:, 1])

    pos_a = np.dstack((grid_x_a, grid_y_a))
    pos_b = np.dstack((grid_x_b, grid_y_b))

    pdf_a = multivariate_normal.pdf(x=pos_a, mean=mean_a_array, cov=cov_a)
    pdf_b = multivariate_normal.pdf(x=pos_b, mean=mean_b_array, cov=cov_b)

    return {
        'pdf' : {
            'a' : pdf_a,
            'b' : pdf_b
        },
        'grid_point' : {
            'a' : {
                'X' : grid_x_a,
                'Y' : grid_y_a
            },
            'b' : {
                'X' : grid_x_b,
                'Y' : grid_y_b
            }
        },
        'mean' : {
            'a' : mean_a_array,
            'b' : mean_b_array
        },
        'cov' : {
            'a' : cov_a,
            'b' : cov_b
        }
    }

def calculateMahalanobis(data, cal_result, vender):
    return mahalanobis(
        u = data,
        v = cal_result['mean'][vender],
        VI = np.linalg.inv(cal_result['cov'][vender])
    )
## ================ cov, rank calculate hander end =====
## ================ draw hander ========================
def platDrawHandler(pca, data_a_length, data_b_length, flag):
    plt.axis('equal')
    sample_total_length = data_a_length + data_b_length
    for count, pca in enumerate(pca):
        if count < data_a_length :
            plt.scatter(pca[0], pca[1], c='red', alpha=0.7)
        elif count < sample_total_length :
            plt.scatter(pca[0], pca[1], c='blue', alpha=0.7)
        else :
            if count == sample_total_length :
                plt.scatter(pca[0], pca[1], c='green', alpha=1)
            else :
                plt.scatter(pca[0], pca[1], c='black', alpha=1)
    plt.show()

def gaussianDrawHandler(X, Y, pdf):
    figure = plt.figure()
    axes = figure.gca(projection='3d')
    axes.plot_surface(X, Y, pdf, rstride=1, cstride=1, cmap='viridis', linewidth=0)
    plt.show()
## ================ draw hander end =====================
## ================ test data classify ==================
def testFileClassifyHandler(eig_pairs, test_data, vender_num):
    test_convert_array = [
        getCalArray(test_data, number)
        for number in range(vender_num)
    ]
    test_pca = getProjection(eig_pairs, test_convert_array, vender_num)
    return test_pca
## ================ test data classify end===============

if __name__ == "__main__":
    file_path = r"E:\Project\gitRepo\mathvision2019\HW8_a"
    file_data_a = readFileHandler("data_a.txt", file_path)
    file_data_b = readFileHandler("data_b.txt", file_path)
    test_data = readFileHandler("test.txt", file_path)
    vender_number = 4

    pca_result = calculateHandler(file_data_a, file_data_b, vender_number)
    gaussian_result = getCalculateGaussian(len(file_data_a), pca_result['pca'])
    test_pca_result = testFileClassifyHandler(
                        pca_result['eig_pairs'], test_data, vender_number
                      )
    # sub space draw    
    platDrawHandler(pca_result['pca'], len(file_data_a), len(file_data_b), False)
    ''' gaussian draw
    gaussianDrawHandler(
        gaussian_result['grid_point']['a']['X'],
        gaussian_result['grid_point']['a']['Y'],
        gaussian_result['pdf']['a']
    )
    '''

    ''' classify test data
    # class a
    print("class a ===========================")
    print(calculateMahalanobis(test_pca_result[0][0], gaussian_result, 'a'))
    print(calculateMahalanobis(test_pca_result[0][1], gaussian_result, 'a'))
    # class b
    print("class b ===========================")
    print(calculateMahalanobis(test_pca_result[0][0], gaussian_result, 'b'))
    print(calculateMahalanobis(test_pca_result[0][1], gaussian_result, 'b'))

    plt_data_test = np.append(pca_result['pca'], test_pca_result, axis=0)
    platDrawHandler(plt_data_test, len(file_data_a), len(file_data_b), True)
    '''
    

