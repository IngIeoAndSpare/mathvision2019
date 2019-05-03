import os
import numpy as np
from PIL import Image
import cv2

## ================ image file handler ========================
def getImageFile(file_path):
   image = cv2.imread(file_path, 0)
   return {
      'image' : image,
      'size' : image.shape
   }

def imageArrayConvert(image_array):
   convert_array = image_array.flatten()
   # print(len(convert_array))
   return convert_array

## ================ image file handler end ====================
## ================ curved approximation ======================
def getCurvedApproximation(image_array, width):
   approximation_array =  np.zeros((len(image_array), 6))
   ## ax^2 + by^2 + cxy + dx + ey + f || ref 30p
   coordinate_x = 0
   coordinate_y = 0
   for index, item in enumerate(approximation_array):
      coordinate_y = int((index - coordinate_x) / width)
      coordinate_x = int(index - width * coordinate_y) % width
      item[0] = np.power(coordinate_x,2)
      item[1] = np.power(coordinate_y,2)
      item[2] = coordinate_x * coordinate_y
      item[3] = coordinate_x
      item[4] = coordinate_y
      item[5] = 1

   ## print(len(approximation_array))
   return approximation_array

## =============== curved approximation end ====================
def getImageCurvedHandler(approximate_array, image, size):
   priv_approx_mat = getPseudoInversMat(approximate_array)
   least_squares = np.matmul(priv_approx_mat, image)
   approximate_value = np.matmul(approximate_array, least_squares)
   source_image = approximate_value - image
   source_image = source_image.reshape(size)

   ret,result_image = cv2.threshold(source_image,11,255,cv2.THRESH_BINARY)

   return result_image

def getPseudoInversMat(array):
   return np.linalg.pinv(array)

if __name__ == '__main__':
   file_path = './hw10_sample.png'
   imageInfo = getImageFile(file_path)
   flatten_image_array = imageArrayConvert(imageInfo['image'])
   curved_approximation_array = getCurvedApproximation(
      flatten_image_array, imageInfo['size'][1]
   )

   result_image = getImageCurvedHandler(
      curved_approximation_array, flatten_image_array, imageInfo['size']
   )

   cv2.imshow("result",result_image)
   cv2.waitKey(0)