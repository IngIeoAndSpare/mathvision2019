const POW_NUM = 2,
      VECTOR_ITEM_SIZE = 3;
const X = 0,
      Y = 1,
      Z = 2;

var originPoint = [
        [-0.5, 0, 2.12132],
        [0.5, 0, 2.12132],
        [0.5, -0.707107, 2.828427],
        [0.5, 0.707107, 2.828427],
        [1,1,1]
    ],
    movePoint = [
        [1.363005, -0.42713, 2.339082],
        [1.748084, 0.437983, 2.017688],
        [2.636461, 0.184843, 2.40071],
        [1.4981, 0.871, 2.8837]
    ];

function Vector (x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

function Point (x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;    
}

module.exports = {
    init : function () {
        let originPointArray = this.getPointArray(originPoint);
        let movePointArray = this.getPointArray(movePoint);

        // P1,2,3를 이용해 R1, R2를 값을 구한 후 P4에 적용하여 P4' 가 나오는지 확인.
        // ref : https://sjwd.tistory.com/34
        // get vector
        let oriFrontVector = originPointArray[0].getVector(originPointArray[1]),
            oriBackVector = originPointArray[0].getVector(originPointArray[2]);

        let moveFrontVector = movePointArray[0].getVector(movePointArray[1]),
            moveBackVector = movePointArray[0].getVector(movePointArray[2]);

        // get normal vector
        let oriNormalVector = oriFrontVector.getCrossProduct(oriBackVector),
            moveNormalVector = moveFrontVector.getCrossProduct(moveBackVector);

        // get rotation value ref : https://darkpgmr.tistory.com/81
        let r1CosValue = this.getCosValue(oriNormalVector, moveNormalVector);
        let r1SinValue = this.getSinValue(r1CosValue);

        // get rotation R1
        let normalVectorForOriMove = oriNormalVector.getCrossProduct(moveNormalVector);
        let oriUnitVector = normalVectorForOriMove.getUnitVector();
        let rotation1 = this.getRotationMatrix(r1CosValue, r1SinValue, oriUnitVector);
        
        // check R1
        console.log('======= check R1 =======');
        moveNormalVector.printVectorInfo('moveNormal');
        let checkNormal = this.getMatrixMultiplyArray(
            rotation1, oriNormalVector.convertVectorToArray(true)
        );
        console.log(checkNormal);
        

        // get rotation R2
        let moveUnitVector = moveNormalVector.getUnitVector();
        let outInterMetrix = this.getMatrixMultiplyArray(
                rotation1,
                oriBackVector.convertVectorToArray(true)
        );
        let outInterVector = this.convertMatrixToVector(outInterMetrix, true);
        let r2CosValue = this.getCosValue(outInterVector, moveBackVector);
        let r2SinValue = this.getSinValue(r2CosValue);
        let rotation2 = this.getRotationMatrix(r2CosValue, r2SinValue, moveUnitVector);
        
        //check R2
        console.log('======= check R2 =======');
        moveBackVector.printVectorInfo('moveBackVector');
        let checkOutMetrix = outInterVector.convertVectorToArray(true);
        let checkR2 = this.getMatrixMultiplyArray(
            rotation2, checkOutMetrix
        );
        console.log(checkR2);

        // check rotation 1, 2
        this.checkRotation(
            rotation1,
            rotation2,
            originPointArray[4].convertPointToArray(true),
            originPointArray[0].convertPointToArray(true),
            movePointArray[0].convertPointToArray(true),
            movePointArray[2].convertPointToArray(true),
            false
        )
    },
    /**
     * return point object array.
     * @param {array} sourcePointArray floot value array. must 1 dimension. 
     */
    getPointArray : function (sourcePointArray) {
        let pointArray = [];
        for (let pointItem of sourcePointArray) {
            pointArray.push(
                new Point(pointItem[X], pointItem[Y], pointItem[Z])
            );
        }
        return pointArray;
    },
    /**
     * get cos value
     * @param {Vector} sourceVector 
     * @param {Vector} targetVector 
     */
    getCosValue : function (sourceVector, targetVector) {
        return (
            sourceVector.getDotProduct(targetVector) /
            sourceVector.getVectorLength() * targetVector.getVectorLength()
        );
    },
    /**
     * get sin value
     * @param {float} cosValue cosin Value.
     */
    getSinValue : function (cosValue) {
        return Math.sqrt(1 - Math.pow(cosValue, POW_NUM));
    },
    /**
     * get rotation matrix.
     * @param {floot} cosValue cosValue For degree
     * @param {floot} sinValue sinValue For degree
     * @param {Vector} uVector unit Vector
     */
    getRotationMatrix: function (cosValue, sinValue, uVector) {
        let xPowValue = Math.pow(uVector.x, POW_NUM),
            yPowValue = Math.pow(uVector.y, POW_NUM),
            zPowValue = Math.pow(uVector.z, POW_NUM),
            subCosValue = 1 - cosValue;

        return [
            [
                cosValue + xPowValue * subCosValue,
                uVector.x * uVector.y * subCosValue - uVector.z * sinValue,
                uVector.x * uVector.z * subCosValue + uVector.y * sinValue
            ],
            [
                uVector.y * uVector.x * subCosValue + uVector.z * sinValue,
                cosValue + yPowValue * subCosValue,
                uVector.y * uVector.z * subCosValue - uVector.x * sinValue
            ],
            [
                uVector.z * uVector.x * subCosValue - uVector.y * sinValue,
                uVector.z * uVector.y * subCosValue + uVector.x * sinValue,
                cosValue + zPowValue * subCosValue
            ]
        ];
    },
    /**
     * get matrix multiply array. must all colum be same size. 
     * calculate => sourceMatrix * targetMatrix
     * return array.
     * @param {array} sourceMatrix 
     * @param {array} targetMatrix 
     */
    getMatrixMultiplyArray: function(sourceMatrix, targetMatrix) {
        //XXX: targetMatrixRows는 확인용 변수로 선언됨.
        let sourceMatrixRows = sourceMatrix.length,
            sourceMatrixCols = sourceMatrix[0].length,
            targetMatrixRows = targetMatrix.length,
            targetMatrixCols = targetMatrix[0].length;
        
        let resultArray = new Array(sourceMatrixRows);

        for(let rowNum = 0; rowNum < sourceMatrixRows; ++rowNum) {
            resultArray[rowNum] = new Array(targetMatrixCols);
            for(let colNum = 0; colNum < targetMatrixCols; ++colNum) {
                resultArray[rowNum][colNum] = 0;
                for(let cellNum = 0; cellNum < sourceMatrixCols; ++cellNum) {
                    resultArray[rowNum][colNum] +=
                     sourceMatrix[rowNum][cellNum] * targetMatrix[cellNum][colNum];
                }// cell for end
            }// colNum for end
        } // rowNum for end
        
        return resultArray;
    },
    /**
     * convert matrix to vector object. must matrix size be 3
     * @param {array} sourceMatrix source matrix
     * @param {boolean} flag input matrix category. true => [[i], [i], [i]] || false => [[i, i, i]]
     */
    convertMatrixToVector: function(sourceMatrix, flag) {
        let convertVector = new Vector();
        for(let num = 0; num < VECTOR_ITEM_SIZE; num++) {
            convertVector[String.fromCharCode(num + 120)] = 
                (flag ? sourceMatrix[num][0] : sourceMatrix[0][num])
        }
        return convertVector;
    },
    /**
     * calculate move point use rotation 1, 2. and print result and check anwser point. 
     * @param {array} rotation1 rotation 1 matrix
     * @param {array} rotation2 rotation 2 matrix
     * @param {array} inputPoint target point
     * @param {array} oriFrontPoint ori front point
     * @param {array} moveFrontPoint move front point
     * @param {array} resultPoint anwser point
     */
    checkRotation: function (rotation1, rotation2, inputPoint, oriFrontPoint, moveFrontPoint, resultPoint, flag) {
        let rotationMultipleValue = this.getMatrixMultiplyArray(rotation2, rotation1);
        let decInputOriFrontPoint = this.calArray(inputPoint, oriFrontPoint, false);

        let calculateValue = this.getMatrixMultiplyArray(
            rotationMultipleValue, decInputOriFrontPoint
        );
        let result = this.calArray(calculateValue, moveFrontPoint, true);
        console.log('====== cal result ======');
        console.log(result);
        if(flag) {
            console.log('====== anwser =======');
            console.log(resultPoint);
        }
    },
    /**
     * array calcaulte sum or subtract.
     * @param {array} sourceMatrix 
     * @param {array} targetMatrix 
     * @param {boolean} flag true - sum || false - subtract
     */
    calArray: function (sourceMatrix, targetMatrix, flag) {
        let rows = sourceMatrix.length;
        let cols = sourceMatrix[0].length;
        let resultMatrix = [];
        for(let rowNum = 0; rowNum < rows; rowNum++) {
            let colsMatrix = [];
            for(let colNum = 0; colNum < cols; colNum++) {
                colsMatrix[colNum] = sourceMatrix[rowNum][colNum] +
                 targetMatrix[rowNum][colNum] * (flag ? 1 : -1);
            }
            resultMatrix.push(colsMatrix);
        }
        return resultMatrix;
    }
}

Point.prototype = {
    /**
     * return vector to this_point-target_point
     * @param {Point} targetPoint 
     */
    getVector: function (targetPoint) {
        return new Vector(
            targetPoint.x - this.x,
            targetPoint.y - this.y,
            targetPoint.z - this.z
        );
    },
    /**
     * return length 
     * @param {Point} targetPoint 
     */
    getLength: function (targetPoint) {
        return Math.sqrt(
            Math.pow(targetPoint.x - this.x, POW_NUM) +
            Math.pow(targetPoint.y - this.y, POW_NUM) +
            Math.pow(targetPoint.z - this.z, POW_NUM)
        );
    },
    /**
     * convert point to matrix
     * @param {boolean} flag  true : return column metrix  // false : return row metrix
     */
    convertPointToArray : function (flag) {
        return (
            flag ? [[this.x], [this.y], [this.z]] : 
                   [[this.x, this.y, this.z]]
        );
    }
}

Vector.prototype = {
    /**
     * get vector length. 
     */
    getVectorLength : function () {
        return Math.sqrt(
            Math.pow(this.x, POW_NUM) + Math.pow(this.y, POW_NUM) + Math.pow(this.z, POW_NUM)
        );
    },
    /**
     * get Cross product. thisVector X targetVector
     * @param {Vector} targetVector 
     */
    getCrossProduct : function (targetVector) {
        let x = (this.y * targetVector.z) - (this.z * targetVector.y),
            y = (this.z * targetVector.x) - (this.x * targetVector.z),
            z = (this.x * targetVector.y) - (this.y * targetVector.x);

        return new Vector(x, y, z);
    },
    /**
     * get Dot product value. thisVector * targetVector
     * @param {Vector} targetVector target product vector
     */
    getDotProduct : function (targetVector) {
        return (this.x * targetVector.x) + (this.y * targetVector.y) + (this.z * targetVector.z); 
    },
    /**
     * get unit vector. 
     */
    getUnitVector : function () {
        let vectorLength = this.getVectorLength();
        return new Vector(this.x / vectorLength, this.y / vectorLength, this.z /vectorLength);
    },
    /**
     * print vector x, y, z info
     * @param {string} name print for vector name.
     */
    printVectorInfo : function (name) {
        console.log(name + ":   x => " + this.x + "   y => " + this.y + "    z => " + this.z);
    },
    /**
     * convert vector to matrix
     * @param {boolean} flag  true : return column metrix  // false : return row metrix
     */
    convertVectorToArray : function (flag) {
        return (
            flag ? [[this.x], [this.y], [this.z]] : 
                   [[this.x, this.y, this.z]]
        );
    }
}
