import numpy as np
import matplotlib.pylab as plt
from sympy import *
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits import mplot3d

## ==================== item init =========================
def initItemHandler(x_start, x_end, y_start, y_end, item_count):
    x_array = getArray(x_start, x_end, 30)
    y_array = getArray(y_start, y_end, 30)
    x_array, y_array = np.meshgrid(x_array, y_array)
    z_array = scalarFunction(x_array, y_array)

    return x_array, y_array, z_array

def scalarFunction(x_array, y_array):
    return np.sin(x_array + y_array - 1) + pow((x_array - y_array - 1), 2) - (1.5 * x_array) + (2.5 * y_array) + 1

def getArray(value_start, value_end, item_count):
    return np.linspace(start= value_start, stop = value_end, num = item_count)

def getMeshgrid(x_array, y_array):
    return np.meshgrid(x_array, y_array)
## ==================== item init end======================
## ==================== draw start ========================
def drawGraphicHandler(x_array, y_array, z_array):
    figure = plt.figure()
    axes = figure.gca(projection='3d')
    axes.plot_surface(
        x_array, y_array, z_array,
        rstride=1, cstride=1, cmap='viridis',
        linewidth=0
    )

    axes.set_xlabel('x')
    axes.set_ylabel('y')
    axes.set_zlabel('z')

    #plt.show()

## ==================== draw end ==========================
## =================== gradient descent ===================
def gradientDescentHandler(scalar_expression, z_array, start_point, lamda_value):
    
    print('x = {}, y = {}'.format(start_point['x'],start_point['y']))
    x, y = symbols('x y')
    diff_gradient_expression = [scalar_expression.diff(x), scalar_expression.diff(y)]
    init_gradient = getGradient(start_point['x'], start_point['y'], diff_gradient_expression)


def getGradient(point_x, point_y, diff_gradient_expression):
    return np.array([
        diff_gradient_expression[0].subs([(x, point_x), (y, point_y)]),
        diff_gradient_expression[1].subs([(x, start_point['x']), (y, start_point['y'])])
    ])

def getNextPoint(gradient_array, lamda_value, ahead_point):
    return np.array([start_point['x'], start_point['y']]) - lamda_value * gradient_array

def getHessianMetrix(diff_gradient_expression):
    return [
        [diff_gradient_expression[0].deff(x), diff_gradient_expression[0].deff(y)],
        [diff_gradient_expression[1].deff(x), diff_gradient_expression[1].deff(y)]
    ]


## =================== gradient descent end ===============

if __name__ == '__main__':
    x_start = -1
    x_end = 5
    y_start = -3
    y_end = 4

    x_array, y_array, z_array = initItemHandler(x_start, x_end, y_start, y_end, 30)
    drawGraphicHandler(x_array, y_array, z_array)

    ## init expression
    x, y = symbols('x y')
    scalar_expression = sin(x + y - 1) * (x - y - 1) ** 2 - (1.5 * x) + (2.5 * y ) + 1
    ## gradient 
    start_point = {
        'x' : np.random.randint(x_start, x_end),
        'y' : np.random.randint(y_start, y_end)
    }
    lamda_value = 0.02
    gradientDescentHandler(scalar_expression, z_array, start_point, lamda_value)