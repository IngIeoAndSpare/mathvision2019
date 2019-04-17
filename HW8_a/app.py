import os

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
    item_array = [float(item) for item in string_line.split(',')]
    return item_array

def readFileHandler(file_name):
    file_path = r"E:\Project\gitRepo\mathvision2019\HW8_a"
    changeFilePath(file_path)
    file_content = readFile(file_name)

    return file_content
## ================ file hander method end ================
## ================ file hander method ====================

if __name__ == "__main__":
    fileData = readFileHandler("data_a.txt")