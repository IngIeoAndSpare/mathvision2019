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
def drawGraphicHandler(x_array, y_array, z_array, point_array = None):
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

    if point_array is not None:
        for point_item in enumerate(point_array):
            point = point_item[1]
            print(point)
            axes.plot([point[0]], [point[1]], [point[2]], marker='o', color='red')
    
    plt.show()

## ==================== draw end ==========================
## =================== gradient descent ===================
def gradientDescentHandler(scalar_expression, start_point, lamda_value, iteration):
    
    point_array = []
    print('x = {}, y = {}'.format(start_point['x'], start_point['y']))
    x, y = symbols('x y')
    diff_gradient_expression = [scalar_expression.diff(x), scalar_expression.diff(y)]
    init_gradient = getGradient(start_point['x'], start_point['y'], diff_gradient_expression)
    point_array.append(
        getNextPoint(init_gradient, lamda_value, start_point['x'], start_point['y'], scalar_expression)
    )
    
    for count in range(iteration):
        next_gradient = getGradient(point_array[count][0], point_array[count][1], diff_gradient_expression)
        point_array.append(
            getNextPoint(
                next_gradient, lamda_value, point_array[count][0], point_array[count][1], scalar_expression
            )
        )
    
    return point_array

def getGradient(point_x, point_y, diff_gradient_expression):
    return np.array([
        diff_gradient_expression[0].subs([(x, point_x), (y, point_y)]),
        diff_gradient_expression[1].subs([(x, point_x), (y, point_y)])
    ], dtype='float')

def getNextPoint(gradient_array, lamda_value, ahead_point_x, ahead_point_y, scalar_expression):
    next_point = np.array([ahead_point_x, ahead_point_y]) - lamda_value * gradient_array
    z_value = scalar_expression.subs([(x, next_point[0]), (y, next_point[1])])
    
    return [next_point[0], next_point[1], z_value]

## =================== gradient descent end ===============
## =================== newton's ===========================
def newtonMinimizationHandler(scalar_expression, start_point, iteration):
    point_array = []
    print('x = {}, y = {}'.format(start_point['x'], start_point['y']))

    x, y = symbols('x y')
    diff_gradient_expression = [scalar_expression.diff(x), scalar_expression.diff(y)]
    init_gradient = getGradient(start_point['x'], start_point['y'], diff_gradient_expression)
    hessian_expression = getHessianMetrix(diff_gradient_expression)
    init_hessian_array = getHessianValue(hessian_expression, start_point['x'], start_point['y'])

    point_array.append(
        nextNewtonMinPoint(
            init_hessian_array, init_gradient, scalar_expression, start_point['x'], start_point['y']
        )
    )
    for count in range(iteration):
        next_gradient = getGradient(point_array[count][0], point_array[count][1], diff_gradient_expression)
        next_hessian_array = getHessianValue(hessian_expression, point_array[count][0], point_array[count][1])
        point_array.append(
            nextNewtonMinPoint(
                next_hessian_array, next_gradient, scalar_expression, point_array[count][0], point_array[count][1]
            )
        )

    return point_array

def getHessianMetrix(diff_gradient_expression):
    return [
        [diff_gradient_expression[0].diff(x), diff_gradient_expression[0].diff(y)],
        [diff_gradient_expression[1].diff(x), diff_gradient_expression[1].diff(y)]
    ]

def getHessianValue(hessian_array, point_x, point_y):
    return np.array([
        [
            hessian_array[0][0].subs([(x, point_x), (y, point_y)]),
            hessian_array[0][1].subs([(x, point_x), (y, point_y)])
        ],
        [
            hessian_array[1][0].subs([(x, point_x), (y, point_y)]),
            hessian_array[1][1].subs([(x, point_x), (y, point_y)])
        ]
    ], dtype = 'float')

def nextNewtonMinPoint(hassian_array, gradient, scalar_expression, point_x, point_y):
    u, s, vh = np.linalg.svd(hassian_array)
    s = np.diag(np.abs(s))

    convert_array = np.linalg.inv(np.matmul(np.matmul(u, s), vh))
    next_point = [point_x, point_y] - np.matmul(convert_array, gradient)
    z_value = scalar_expression.subs([(x, next_point[0]), (y, next_point[1])])

    return [next_point[0], next_point[1], z_value]
# =========================================================


if __name__ == '__main__':
    x_start = -1
    x_end = 5
    y_start = -3
    y_end = 4

    x_array, y_array, z_array = initItemHandler(x_start, x_end, y_start, y_end, 30)
    # drawGraphicHandler(x_array, y_array, z_array)

    ## init expression
    x, y = symbols('x y')
    scalar_expression = sin(x + y - 1) * (x - y - 1) ** 2 - (1.5 * x) + (2.5 * y ) + 1
    ## gradient 
    start_point = {
        'x' : np.random.randint(x_start, x_end),
        'y' : np.random.randint(y_start, y_end)
    }
    lamda_value = 0.01
    iteration = 20
    # gradient_point_array = gradientDescentHandler(scalar_expression, start_point, lamda_value, iteration)
    # drawGraphicHandler(x_array, y_array, z_array, gradient_point_array)
    ## newton's scalar_expression, start_point, iteration
    
    newton_mini_point = newtonMinimizationHandler(scalar_expression, start_point, iteration)
    drawGraphicHandler(x_array, y_array, z_array, newton_mini_point)
